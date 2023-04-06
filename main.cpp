#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const float PI = 3.14159265536;

void generate_a(float freq, short amp, short phase, int nsamples) {
    ofstream a;
    a.open("a.dat", ios::binary | ios::out);

    // The number of samples is not explicitly stated, and it may vary. 
    // An array whose size can't be known by the compiler can't go into the stack memory, so we dynamically allocate samples[].
    float *samples = new float[nsamples];
    float dt = (1. / freq) / 20.0;

    for (int i = 0; i < nsamples; i++) {
        samples[i] = (float)amp * sin(2 * PI * freq * i * dt);
    }

    // Let's say samples[] starts from the 1024th cell in the memory. A float takes up 4 cells, so samples[0] uses 1024~1027th cells, samples[1] uses 1028~1031st cells, and so on.
    // The 'write' method copies what's in the memory into where a.dat, byte by byte. 
    // The method will first copy what's in the 1024th cell, and call for 'next cell' to copy what's in the 1025th cell. 
    // But if one calls a 'next cell' of a float pointer that points at 1024th cell, they will get the 1028th, not 1025. (because float takes up 4 cells)
    // That's why 'write' method only acceps char pointers, and we need to typecast (or, 'gaslight') 'samples' as a char pointer, to succesfully copy (sizeof(float) * nsamples) bytes into a.dat byte by byte (because char takes up 1 byte)
    // This is my brain-ficial
    a.write((char*)samples, sizeof(float) * nsamples);

    // float pointer 'samples' will be automatically deleted (or 'popped') from the stack memory when this function terminates.
    // However, the array of floats in the heap memory that 'samples' was 'pointing to' will not be automatically deleted. If we just leave it, it will be there forever untill the whole program terminates.
    // This is called a 'memory leak', and it has to be avoided at all times.
    delete[] samples;
    a.close();
    return;
}



void generate_b(float old_freq, float new_freq, int nsamples) {
    ifstream a;
    a.open("a.dat", ios::binary | ios::in);
    // old dt
    float dt = (1. / old_freq) / 20.0;

    float *samples = new float[nsamples];
    // read first 12 bytes from a
    a.read((char*)samples, 12);
    
    // just to make sure next read() starts from the 12th byte. This is unlikely to be necessary.
    a.seekg(0, a.beg);
    a.seekg(12);

    // read remaining bytes from a
    a.read(((char*)samples) + 12, sizeof(float) * nsamples - 12);
    a.close();

    ofstream b;
    b.open("b.dat", ios::binary | ios::out);
    for (int i = 12; i < nsamples; i++) {
        samples[i] /= sin(2 * PI * old_freq * i * dt);
        samples[i] *= sin(2 * PI * new_freq * i * dt);
    }
    
    b.write((char*)samples, sizeof(float) * nsamples);

    b.close();

    delete[] samples;
    return;
}

int main() {
    
}
