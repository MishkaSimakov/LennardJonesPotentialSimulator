#ifndef GPUTEST_PROGRAM_H
#define GPUTEST_PROGRAM_H

#include <OpenCL/opencl.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define WIDTH 8
#define HEIGHT 8

#define DATA_SIZE (WIDTH * HEIGHT)
#define BOX_SIZE 1500

class Program {
private:
    cl_mem m_input, m_output;
    cl_program m_program;
    cl_kernel m_kernel;
    cl_command_queue m_commands;
    cl_context m_context;

    cl_device_id m_device_id;

    int m_error_code;
public:
    Program() {
        clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, 1, &m_device_id, nullptr);
        m_context = clCreateContext(nullptr, 1, &m_device_id, nullptr, nullptr, &m_error_code);
        m_commands = clCreateCommandQueue(m_context, m_device_id, 0, &m_error_code);

        m_input = clCreateBuffer(m_context, CL_MEM_READ_ONLY, sizeof(cl_float8) * DATA_SIZE, nullptr, nullptr);
        m_output = clCreateBuffer(m_context, CL_MEM_WRITE_ONLY, sizeof(cl_float8) * DATA_SIZE, nullptr, nullptr);
    }

    void loadProgram(const char *filename) {
        std::ifstream ifs;
        ifs.open(filename);

        if (!ifs.is_open()) return;

        std::stringstream strStream;
        strStream << ifs.rdbuf();

        std::string program = strStream.str();
        const char *programLink = program.c_str();

        ifs.close();

        m_program = clCreateProgramWithSource(m_context, 1, &programLink, nullptr, &m_error_code);
        clBuildProgram(m_program, 0, nullptr, nullptr, nullptr, nullptr);
        m_kernel = clCreateKernel(m_program, "program", &m_error_code);
    }

    template<class data_T>
    void setInputData(data_T *data, float box_size) {
        m_error_code = clEnqueueWriteBuffer(
                m_commands, m_input, CL_TRUE, 0, sizeof(data_T) * DATA_SIZE, data, 0, nullptr, nullptr
        );

        clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_input);
        clSetKernelArg(m_kernel, 1, sizeof(cl_mem), &m_output);

        unsigned int count = DATA_SIZE;
        clSetKernelArg(m_kernel, 2, sizeof(unsigned int), &count);

        clSetKernelArg(m_kernel, 3, sizeof(float), &box_size);
        size_t local;
        clGetKernelWorkGroupInfo(m_kernel, m_device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, nullptr);
        size_t global = count;
        clEnqueueNDRangeKernel(m_commands, m_kernel, 1, nullptr, &global, &local, 0, nullptr, nullptr);
    }

    void calculate() {
        clFinish(m_commands);
    }

    template<class data_T>
    void getResult(data_T *result) {
        clEnqueueReadBuffer(m_commands, m_output, CL_TRUE, 0, sizeof(data_T) * DATA_SIZE, result, 0, nullptr, nullptr);
    }

    virtual ~Program() {
        clReleaseMemObject(m_input);
        clReleaseMemObject(m_output);
        clReleaseProgram(m_program);
        clReleaseKernel(m_kernel);
        clReleaseCommandQueue(m_commands);
        clReleaseContext(m_context);
    }
};


#endif //GPUTEST_PROGRAM_H
