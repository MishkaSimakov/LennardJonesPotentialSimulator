float wall_force(float distance) {
    if (distance <= 0) return 1;

    if (distance > 2) return 0;

    return 1 / distance;
}

__kernel void program(__global float8* input, __global float8* output, const unsigned int count, const float box_size) {
    int i = get_global_id(0);

    if (i < count) {
        const float SIGMA = 100;
        const float EPSILON = 0.0000001;

        const float SIGMA_SIXTH_POW = pow(SIGMA, 6);

        output[i] = (float8){0, 0, 0, input[i].s012, input[i].s6, input[i].s7};

        float3 force = (float3){0, 0, 0};

        for (int j = 0; j < count; ++j) {
            if (j == i) continue;

            float distance = length(input[i].s012 - input[j].s012);

            if (distance >= 2.5 * SIGMA) continue;

            float f = -24 * EPSILON * SIGMA_SIXTH_POW *
                       (pow(distance, 6) - 2 * SIGMA_SIXTH_POW)
                       / pow(distance, 14);

            float3 rkn = input[i].s012 - input[j].s012;

            force += f * rkn;
        }

        float3 translation = input[i].s012 - input[i].s345;

        if ((input[i].s0 <= -box_size / 2 && translation.x <= 0) || (input[i].s0 >= box_size / 2 && translation.x >= 0)) {
            translation.x *= -1;
            force.x *= -1;
        }

        if ((input[i].s1 <= -box_size / 2 && translation.y <= 0) || (input[i].s1 >= box_size / 2 && translation.y >= 0)) {
            translation.y *= -1;
            force.y *= -1;
        }
        if ((input[i].s2 <= -box_size / 2 && translation.z <= 0) || (input[i].s2 >= box_size / 2 && translation.z >= 0)) {
            translation.z *= -1;
            force.z *= -1;
        }

        force *= 1.5;

        output[i].s012 = input[i].s012 + translation + force / input[i].s6 * input[i].s7 * input[i].s7;
    }
}