/*
 * Author: Grzegorz Milka <grzegorzmilka@gmail.com>
 * Date: 2013-04-20
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CL/cl.h>

#define STRING_TYPE 0
#define CL_UINT_TYPE 1
#define CL_BOOL 2

typedef enum {
    STRING_T,
    CL_UINT_T,
    CL_ULONG_T,
    CL_BOOL_T,
    CL_DEVICE_TYPE_T
} data_format_t;

int getAndDisplayProfileInfo(cl_platform_id platform,
        cl_platform_info param_name,
        char *param_readable_name);

int getAndDisplayDeviceInfo(cl_device_id device,
        cl_device_info param_name,
        char *param_readable_name,
        int param_type);

int getAndDisplayDevices(cl_platform_id platform);

void printDataOfGivenType(void* data, data_format_t type);

int main() {
    /* Host data structures */
    cl_platform_id *platforms;
    cl_uint num_platforms;
    cl_int i, err;

    /* Find number of platforms */
    err = clGetPlatformIDs(1, NULL, &num_platforms);
    if (err < 0) {
        perror("Couldn't find any platforms.");
        exit(1);
    }

    /* Access all installed platforms */
    platforms = (cl_platform_id*)
        malloc(sizeof(cl_platform_id) * num_platforms);
    clGetPlatformIDs(num_platforms, platforms, NULL);

    /* Find extensions of all platforms */
    for (i=0; i < num_platforms; i++) {
        /* Profile */
        printf("Platform %d:\n", i);

        err = getAndDisplayProfileInfo(platforms[i], CL_PLATFORM_PROFILE,
                "PLATFORM");
        if (err != 0) {
            break;
        }

        err = getAndDisplayProfileInfo(platforms[i], CL_PLATFORM_NAME, "NAME");
        if (err != 0) {
            break;
        }

        err = getAndDisplayProfileInfo(platforms[i], CL_PLATFORM_VERSION,
                "VERSION");
        if (err != 0) {
            break;
        }

        err = getAndDisplayProfileInfo(platforms[i], CL_PLATFORM_VENDOR,
                "VENDOR");
        if (err != 0) {
            break;
        }

        err = getAndDisplayProfileInfo(platforms[i], CL_PLATFORM_EXTENSIONS,
                "EXTENSIONS");
        if (err != 0) {
            break;
        }

        err = getAndDisplayDevices(platforms[i]);
        if (err != 0) {
            break;
        }
        printf("\n");
    }

    /* Deallocate resources */
    free(platforms);
    return err;
}

int getAndDisplayProfileInfo(cl_platform_id platform,
        cl_platform_info param_name,
        char *param_readable_name) {
    char *info_data;
    size_t info_size;
    cl_int err;

    err = clGetPlatformInfo(platform, param_name, 0, NULL,
            &info_size);
    if (err == CL_INVALID_PLATFORM || err == CL_INVALID_VALUE) {
        fprintf(stderr, "Couldn't read %s data.", param_readable_name);
        return 1;
    }

    info_data = (char*) malloc(info_size);
    err = clGetPlatformInfo(platform, param_name,
            info_size, info_data, NULL);

    if (err == CL_INVALID_PLATFORM || err == CL_INVALID_VALUE) {
        fprintf(stderr, "Couldn't read %s data.", param_readable_name);
        free(info_data);
        return 1;
    }
    printf("  %s: %s\n", param_readable_name, info_data);
    free(info_data);
    return 0;
}

int getAndDisplayDevices(cl_platform_id platform) {
    int i, j;
    cl_int err;
    cl_uint num_devices;
    cl_device_id *devices;
    cl_device_info params[] = {
        CL_DEVICE_NAME,
        CL_DEVICE_TYPE,
        CL_DEVICE_VENDOR,
        CL_DEVICE_ADDRESS_BITS,
        CL_DEVICE_AVAILABLE,
        CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
        CL_DEVICE_GLOBAL_MEM_SIZE,
        CL_DEVICE_LOCAL_MEM_SIZE,
        CL_DEVICE_MAX_CLOCK_FREQUENCY,
        CL_DEVICE_MAX_COMPUTE_UNITS,
        CL_DEVICE_MAX_WORK_GROUP_SIZE,
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        CL_DEVICE_VERSION,
        CL_DRIVER_VERSION
    };

    char* readable_params[] = {
        "CL_DEVICE_NAME",
        "CL_DEVICE_TYPE",
        "CL_DEVICE_VENDOR",
        "CL_DEVICE_ADDRESS_BITS",
        "CL_DEVICE_AVAILABLE",
        "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE",
        "CL_DEVICE_GLOBAL_MEM_SIZE",
        "CL_DEVICE_LOCAL_MEM_SIZE",
        "CL_DEVICE_MAX_CLOCK_FREQUENCY",
        "CL_DEVICE_MAX_COMPUTE_UNITS",
        "CL_DEVICE_MAX_WORK_GROUP_SIZE",
        "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",
        "CL_DEVICE_VERSION",
        "CL_DRIVER_VERSION"
    };

    data_format_t param_types[] = {
        STRING_T,
        CL_DEVICE_TYPE_T,
        STRING_T,
        CL_UINT_T,
        CL_BOOL_T,
        CL_ULONG_T,
        CL_ULONG_T,
        CL_ULONG_T,
        CL_UINT_T,
        CL_UINT_T,
        CL_UINT_T,
        CL_UINT_T,
        STRING_T,
        STRING_T
    };


    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
    if (err != CL_SUCCESS) {
        perror("Couldn't access any devices");
        return 1;
    }

    if (num_devices == 0) {
        printf("No devices were found.\n");
        return 0;
    }

    devices = malloc(sizeof(cl_device_id) * num_devices);
    if (devices == NULL) {
        perror("Couldn't allocate memory for devices");
        return 1;
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
    if (err != CL_SUCCESS) {
        perror("Couldn't get any devices");
        free(devices);
        return 1;
    }

    for (i = 0; i < num_devices; ++i) {
        printf("\n    Device %d:\n", i);
        for (j = 0; j < sizeof(params)/sizeof(cl_device_info); ++j) {
            printf("  ");
            err = getAndDisplayDeviceInfo(devices[i],
                    params[j],
                    readable_params[j],
                    param_types[j]);
            if (err) {
                num_devices = 0;
                break;
            }
        }
    }
    return err;
}

int getAndDisplayDeviceInfo(cl_device_id device,
        cl_device_info param_name,
        char *param_readable_name,
        int param_type) {
    cl_int err;
    void* data;
    size_t param_size;

    err = clGetDeviceInfo(device, param_name, 0, NULL, &param_size);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Couldn't get %s parameter from device.", param_readable_name);
        return 1;
    }

    data = malloc(param_size);
    if (!data) {
        fprintf(stderr, "Couldn't allocate memory for device data.");
        return 1;
    }

    err = clGetDeviceInfo(device, param_name, param_size, data, NULL);
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Couldn't get %s parameter from device.", param_readable_name);
        free(data);
        return 1;
    }

    printf("  %s: ", param_readable_name);
    printDataOfGivenType(data, param_type);
    printf("\n");
    free(data);
    return 0;
}

void printDataOfGivenType(void* data, data_format_t type) {
    if (type == STRING_TYPE) {
        printf("%s", (char*) data);
    } else if (type == CL_UINT_T) {
        printf("%u", *(cl_uint*) data);
    } else if (type == CL_ULONG_T) {
        printf("%lu", *(cl_ulong*) data);
    } else if (type == CL_BOOL_T) {
        printf("%d", *(cl_bool*) data);
    } else if (type == CL_DEVICE_TYPE_T) {
        cl_device_type dev_type = *(cl_device_type*) data;

        if (dev_type & CL_DEVICE_TYPE_CPU) {
            printf("CL_DEVICE_TYPE_CPU; ");
        }

        if (dev_type & CL_DEVICE_TYPE_GPU) {
            printf("CL_DEVICE_TYPE_GPU; ");
        }

        if (dev_type & CL_DEVICE_TYPE_ACCELERATOR) {
            printf("CL_DEVICE_TYPE_ACCELERATOR; ");
        }
    }
}
