#include <stdio.h>
#include <math.h>

  // Calculate L squared for a circular orbit
double L_Squared(double M_geom, double r, double c)
{
    return (M_geom * c * c * r * r)
           / (r - 3.0 * M_geom);
}

// Calculate specific energy for a circular orbit
double circular_E(double M_geom, double r, double c)
{
    return c * c
           * (1.0 - (2.0 * M_geom / r))
           / sqrt(1.0 - (3.0 * M_geom / r));
}

//radial acceleration of the object
double d2r_dtau2(double r, double L, double M_geom, double c)
{
    return (L * L) / (r * r * r)
           - (M_geom * c * c) / (r * r)
           - (3.0 * M_geom * L * L)
             / (r * r * r * r);
}


double dphi_dtau(double L, double r)
{
    return L / (r * r);
}

// Coordinate time rate
double dt_dtau(double E, double M_geom, double r, double c)
{
    return E /
           (c * c * (1.0 - (2.0 * M_geom / r)));
}

int main(void)
{
    //universal constants
    const double G = 6.67430e-11;
    
    const double c = 299792458.0;

    const double solar_mass = 1.98847e30;

    const double M = 66.0e9 * solar_mass;

    const double M_geom = G * M / (c * c);


    // Schwarzschild radius
    const double rs = 2.0 * M_geom;

    //object start pos
    double r = 5.847612e14;
    double r_initial = r;

    double phi = 0.0;
    double dr_dtau = 0.0;
    
    double t = 0.0;
    double tau = 0.0;
    double x = r;
    double y = 0.0;
        

    //time step
    double dtau = 0.01;

    //angular momentum
    double L_squared = L_Squared(M_geom, r, c);

    double L = sqrt(L_squared);

    // Calculate energy
    double E = circular_E(M_geom, r, c);
    
    printf("Schwarzschild radius = %e m\n", rs);
    
    printf("L = %e\n", L);

    printf("E = %e\n", E);

    //enter the simulation baruch hashem
   for (int i = 0; i < 1000000; i++)
{
    double acceleration = d2r_dtau2(r, L, M_geom, c);

    // Update radial velocity
    dr_dtau = dr_dtau + acceleration * dtau;

    r = r + dr_dtau * dtau;

    double angular_velocity = dphi_dtau(L, r);

    double coordinate_time_rate = dt_dtau(E, M_geom, r, c);

    // Update angle
    phi = phi + angular_velocity * dtau;

    t = t + coordinate_time_rate * dtau;

    tau = tau + dtau;

    //position
    double x = r * cos(phi);
    double y = r * sin(phi);


    if (i % 10000 == 0)
    {
        printf(
            "tau = %e s | "
            "t = %e s | "
            "x = %e m | "
            "y = %e m | "
            "r = %.15e m | "
            "delta_r = %.6e m | "
            "phi = %e | "
            "dr/dtau = %e\n",

            tau,
            t,
            x,
            y,
            r,
            r - r_initial,
            phi,
            dr_dtau
        );
    }
}
        return 0;

}