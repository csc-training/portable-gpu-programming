#include <stdio.h>
#include <math.h>
#include <roctx.h>

void create_input(double *f, int nx, int ny) {
    double cx = nx / 2.0;
    double cy = ny / 2.0;
    double sigma2 = 0.05 * nx*ny;  // Width of the Gaussian
    double kx = 20.0 / nx;  // Spatial frequency in x
    double ky = 10.0 / ny;  // Spatial frequency in y

    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            int ind = i * nx + j;

            double dx = j - cx;
            double dy = i - cy;
            double r2 = dx * dx + dy * dy;

            f[ind] = cos(kx * dx + ky * dy) * exp(-r2 / sigma2);
        }
    }
}

int write_array(const char *filename, const double *array, size_t count)
{
    roctxRangePush(__func__);
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        roctxRangePop();
        return 1;
    }

    // Write the array size
    fwrite(&count, sizeof(size_t), 1, file);

    // Write the array data
    size_t written = fwrite(array, sizeof(double), count, file);

    fclose(file);

    if (written != count) {
        fprintf(stderr, "Failed to write all elements to file\n");
        roctxRangePop();
        return 2;
    }
    roctxRangePop();

    return 0;
}
