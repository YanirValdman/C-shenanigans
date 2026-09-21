#include <stdio.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <stdlib.h>

double PixelNDCx(double *Pixelx, double *ImageWidth)
{
    return (*Pixelx + 0.5) / *ImageWidth;
}

double PixelNDCy(double *Pixely, double *ImageHeight)
{
    return (*Pixely + 0.5) / *ImageHeight;
}

double PixelScreenx(double *PixelNDCx)
{
    return 2.0 * *PixelNDCx - 1.0;
}

double PixelScreeny(double *PixelNDCy)
{
    return 1.0 - 2.0 * *PixelNDCy;
}

double ImageAspectRatio(double *ImageWidth, double *ImageHeight)
{
    return *ImageWidth / *ImageHeight;
}

double PixelCamerax(double *PixelScreenx, double *ImageAspectRatio)
{
    return *PixelScreenx * *ImageAspectRatio;
}

double PixelCameray(double *PixelScreeny)
{
    return *PixelScreeny;
}

double d2u_dphi2(double u, double M_geom)
{
    return 3.0 * M_geom * u * u - u;
}

double StarBrightness(double x, double y)
{
    double scale = 35.0;

    double sx = floor(x * scale);
    double sy = floor(y * scale);

    double value = sin(sx * 12.9898 + sy * 78.233) * 43758.5453;

    value -= floor(value);

    if(value > 0.997)
    {
        return 1.0;
    }

    if(value > 0.985)
    {
        return 0.35;
    }

    return 0.0;
}

void StarColor(double x, double y, double brightness, Uint32 *color)
{
    double red = 4.0 + 255.0 * brightness;
    double green = 7.0 + 230.0 * brightness;
    double blue = 18.0 + 180.0 * brightness;

    if(red > 255.0) red = 255.0;
    if(green > 255.0) green = 255.0;
    if(blue > 255.0) blue = 255.0;

    *color = 0xFF000000 |
             ((Uint32)red << 16) |
             ((Uint32)green << 8) |
             (Uint32)blue;
}

int main(void)
{
    const int image_width = 640;
    const int image_height = 480;

    const double G = 6.67430e-11;
    const double c = 299792458.0;
    const double solar_mass = 1.98847e30;

    const double M = 66.0e9 * solar_mass;
    const double M_geom = G * M / (c * c);

    const double rs = 2.0 * M_geom;
    const double photon_sphere = 3.0 * M_geom;

    const double critical_impact = 3.0 * sqrt(3.0) * M_geom;

    const double camera_distance = 1.0e16;

    const double fov = 60.0 * M_PI / 180.0;
    const double fov_scale = tan(fov * 0.5);

    double width = image_width;
    double height = image_height;

    double aspect_ratio = ImageAspectRatio(&width, &height);

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Black Hole",
        image_width,
        image_height,
        0
    );

    if(window == NULL)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if(renderer == NULL)
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        image_width,
        image_height
    );

    if(texture == NULL)
    {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    Uint32 *pixels = malloc(image_width * image_height * sizeof(Uint32));

    if(pixels == NULL)
    {
        printf("Pixel allocation failed.\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    printf("M_geom = %e m\n", M_geom);
    printf("Schwarzschild radius = %e m\n", rs);
    printf("Photon sphere = %e m\n", photon_sphere);
    printf("Critical impact parameter = %e m\n", critical_impact);

    printf("Rendering %d x %d...\n", image_width, image_height);

    for(int py = 0; py < image_height; py++)
    {
        printf("\rRendering: %d / %d", py + 1, image_height);
        fflush(stdout);

        for(int px = 0; px < image_width; px++)
        {
            double pixel_x = px;
            double pixel_y = py;

            double ndc_x = PixelNDCx(&pixel_x, &width);
            double ndc_y = PixelNDCy(&pixel_y, &height);

            double screen_x = PixelScreenx(&ndc_x);
            double screen_y = PixelScreeny(&ndc_y);

            double camera_x = PixelCamerax(&screen_x, &aspect_ratio);
            double camera_y = PixelCameray(&screen_y);

            camera_x *= fov_scale;
            camera_y *= fov_scale;

            double ray_x = camera_x;
            double ray_y = camera_y;
            double ray_z = -1.0;

            double ray_length = sqrt(
                ray_x * ray_x +
                ray_y * ray_y +
                ray_z * ray_z
            );

            ray_x /= ray_length;
            ray_y /= ray_length;
            ray_z /= ray_length;

            double impact_parameter = camera_distance * sqrt(
                ray_x * ray_x +
                ray_y * ray_y
            );

            int hit_black_hole = 0;

            double final_x = ray_x;
            double final_y = ray_y;

            if(impact_parameter <= critical_impact)
            {
                hit_black_hole = 1;
            }
            else
            {
                double r = camera_distance;
                double u = 1.0 / r;

                double v_squared =
                    (1.0 / (impact_parameter * impact_parameter))
                    - (u * u)
                    + (2.0 * M_geom * u * u * u);

                if(v_squared >= 0.0)
                {
                    double v = sqrt(v_squared);
                    double phi = 0.0;
                    double dphi = 0.00001;

                    for(int i = 0; i < 50000; i++)
                    {
                        double acceleration = d2u_dphi2(u, M_geom);

                        v += acceleration * dphi;
                        u += v * dphi;
                        phi += dphi;

                        if(u <= 0.0)
                        {
                            break;
                        }

                        r = 1.0 / u;

                        if(r <= rs)
                        {
                            hit_black_hole = 1;
                            break;
                        }

                        if(r > camera_distance)
                        {
                            final_x = cos(phi);
                            final_y = sin(phi);
                            break;
                        }
                    }
                }
            }

            Uint32 color;

            if(hit_black_hole)
            {
                color = 0xFF000000;
            }
            else
            {
                double star_x = final_x;
                double star_y = final_y;

                double brightness = StarBrightness(star_x, star_y);

                StarColor(
                    star_x,
                    star_y,
                    brightness,
                    &color
                );
            }

            pixels[py * image_width + px] = color;
        }
    }

    printf("\nRendering finished.\n");

    SDL_UpdateTexture(
        texture,
        NULL,
        pixels,
        image_width * sizeof(Uint32)
    );

    int running = 1;

    while(running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
            {
                running = 0;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    free(pixels);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
