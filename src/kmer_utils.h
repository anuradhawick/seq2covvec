#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <fstream>
#include <string>
#include <mutex>
#include <cmath>

using namespace std;

inline u_int64_t revComp(u_int64_t x, u_int64_t k_size)
{
    u_int64_t res = x;

    res = ((res >> 2 & 0x3333333333333333) | (res & 0x3333333333333333) << 2);
    res = ((res >> 4 & 0x0F0F0F0F0F0F0F0F) | (res & 0x0F0F0F0F0F0F0F0F) << 4);
    res = ((res >> 8 & 0x00FF00FF00FF00FF) | (res & 0x00FF00FF00FF00FF) << 8);
    res = ((res >> 16 & 0x0000FFFF0000FFFF) | (res & 0x0000FFFF0000FFFF) << 16);
    res = ((res >> 32 & 0x00000000FFFFFFFF) | (res & 0x00000000FFFFFFFF) << 32);
    res = res ^ 0xAAAAAAAAAAAAAAAA;

    return (res >> (2ULL * (32ULL - k_size)));
}

inline double *line_to_vec(string &line, unordered_map<u_int64_t, atomic<u_int32_t>> &allKmers, u_int64_t k_size, long bin_size, int bins)
{
    double *counts = new double[bins];
    long sum = 0, count, pos;
    u_int64_t len = 0;
    u_int64_t val = 0, rev = 0,min_compl = 0;
    u_int64_t mask = (1ULL<<(2ULL * k_size)) - 1;

    // to avoid garbage memory
    for (int i = 0; i < bins; i++)
    {
        counts[i] = 0;
    }

    for (size_t i = 0; i < line.length(); i++)
    {
        if (!(line[i] == 'A' || line[i] == 'C' || line[i] == 'G' || line[i] == 'T'))
        {
            val = 0;
            len = 0;
            continue;
        }

        val = (val << 2);
        val = val & mask;
        val += (line[i] >> 1 & 3ULL);
        len++;

        if (len == k_size)
        {
            // use val as the kmer for counting
            len--;
            // get min complement
            rev = revComp(val, k_size);
            min_compl = min(val, rev);

            if (allKmers.find(min_compl) == allKmers.end())
            {
                count = 0;
            }
            else
            {
                count = allKmers[min_compl];
            }

            count = count < 2 ? 0: count;
            pos = (count / bin_size) - 1;

            if (count <= bin_size)
            {
                counts[0]++;
            }
            else if (pos < bins && pos > 0)
            {
                counts[pos]++;
            }
            else
            {
                counts[bins - 1]++;
            }
            sum++;
        }
    }

    if (sum > 0)
    {
        for (int i = 0; i < bins; i++)
        {
            counts[i] /= sum;
            if (counts[i] < 1e-4)
            {
                counts[i] = 0;
            }
        }
    }

    return counts;
}

inline void writeKmerFile(string filename, vector<atomic<u_int32_t>> &kmers)
{
    ofstream output;
    output.open(filename, ios::out);
    u_int64_t size = kmers.size();
    output.write(reinterpret_cast<char const*>(&size), sizeof(size));
    output.write(reinterpret_cast<char const*>(kmers.data()), kmers.size() * sizeof(kmers[0]));
    output.close();
}

inline vector<atomic<u_int32_t>> readKmerFile(string filename)
{
    ifstream input;
    u_int64_t size;
    input.open(filename);

    input.read(reinterpret_cast<char*>(&size), sizeof(size));
    vector<atomic<u_int32_t>> kmers(size);

    input.read(reinterpret_cast<char*>(kmers.data()), kmers.size() * sizeof(kmers[0]));
    input.close();

    return kmers;
}

inline mutex hash_mux;
inline void line_to_kmer_counts(string &line, unordered_map<u_int64_t, atomic<u_int32_t>> &all_kmers, u_int64_t k_size)
{
    u_int64_t len = 0;
    u_int64_t val = 0, rev = 0, min_compl;
    u_int64_t mask = (1ULL<<(2ULL * k_size)) - 1;
    u_int32_t oval;

    for (size_t i = 0; i < line.length(); i++)
    {
        if (!(line[i] == 'A' || line[i] == 'C' || line[i] == 'G' || line[i] == 'T'))
        {
            val = 0;
            len = 0;
            continue;
        }

        val = (val << 2);
        val = val & mask;
        val += (line[i] >> 1 & 3ULL);
        len++;

        if (len == k_size)
        {
            // pick the smallest of both
            rev = revComp(val, k_size);
            min_compl = min(rev, val);
            
            // add val to hash if not there (singleton pattern)
            if (all_kmers.find(min_compl) == all_kmers.end())
            {
                {
                    unique_lock<mutex> lock(hash_mux);
                    if (all_kmers.find(min_compl) == all_kmers.end())
                    {
                        all_kmers.emplace(min_compl, 0);
                    }
                }
            }
            
            // use val as the kmer for counting
            len--;
            // record original value from index
            oval = all_kmers[min_compl];
            
            // CAS
            while (!all_kmers[min_compl].compare_exchange_weak(oval, oval + 1))
            {
            };
        }
    }
}
