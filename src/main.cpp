#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unordered_map>
#include "io_utils.h"
#include "kmer_utils.h"

using namespace std;

queue<string> reads_queue;
mutex mux;
condition_variable condition;
volatile bool terminate_threads;

void processLinesBatch_counter(vector<string> &batch, unordered_map<u_int64_t, atomic<u_int32_t>> &all_kmers, u_int64_t k_size, int threads)
{
    vector<vector<u_int64_t>> batch_results(batch.size());

#pragma omp parallel for num_threads(threads) schedule(dynamic, 1)
    for (size_t i = 0; i < batch.size(); i++)
    {
        line_to_kmer_counts(batch[i], all_kmers, k_size);
    }

    batch_results.clear();
}

void off_load_process_counter(unordered_map<u_int64_t, atomic<u_int32_t>> &all_kmers, u_int64_t k_size, int &threads)
{
    string seq;
    vector<string> batch;

    while (true)
    {
        {
            unique_lock<mutex> lock(mux);

            while (reads_queue.size() > 0)
            {
                seq = reads_queue.front();
                batch.push_back(seq);
                reads_queue.pop();

                if (batch.size() == 10000)
                {
                    break;
                }
            }
        }

        condition.notify_all();

        if (batch.size() > 0)
        {
            processLinesBatch_counter(batch, all_kmers, k_size, threads);
            batch.clear();
        }

        {
            unique_lock<mutex> lock(mux);
            if (terminate_threads && reads_queue.size() == 0)
            {
                break;
            }
        }
    }
}

void io_thread_counter(string &file_path)
{
    SeqReader reader(file_path);
    Seq seq;
    int count = 0;

    while (reader.get_seq(seq))
    {
        {
            unique_lock<mutex> lock(mux);
            condition.wait(lock, [] { return reads_queue.size() < 50000; });
            reads_queue.push(seq.data);
        }
        count++;
        if (count % 10000 == 0)
        {
            cout << "Loaded Reads " << count << "       \r" << flush;
        }
    }
    cout << "Loaded Reads " << count << "       \r" << flush;
    cout << endl;

    terminate_threads = true;
}

void processLinesBatch(vector<string> &linesBatch, unordered_map<u_int64_t, atomic<u_int32_t>> &allKmers, string &output_path, u_int64_t k_size, int threads, long bin_size, int bins)
{
    vector<double *> batchAnswers(linesBatch.size());

#pragma omp parallel for num_threads(threads) schedule(dynamic, 1)
    for (uint i = 0; i < linesBatch.size(); i++)
    {
        batchAnswers[i] = line_to_vec(linesBatch[i], allKmers, k_size, bin_size, bins);
    }

    ofstream output;
    output.open(output_path, ios::out | ios::app);
    string results = "";

    for (uint i = 0; i < linesBatch.size(); i++)
    {
        for (int j = 0; j < bins; j++)
        {
            results += to_string(batchAnswers[i][j]);

            if (j < bins - 1)
            {
                results += " ";
            }
        }

        results += "\n";

        // releasing pointer memory
        delete[] batchAnswers[i];
    }

    output << results;
    batchAnswers.clear();
    output.close();
}

void off_load_process(string &output, unordered_map<u_int64_t, atomic<u_int32_t>> &all_kmers, int &threads, u_int64_t k_size, long bin_size, int bins)
{
    string seq;
    vector<string> batch;

    while (true)
    {
        {
            unique_lock<mutex> lock(mux);

            while (reads_queue.size() > 0)
            {
                seq = reads_queue.front();
                batch.push_back(seq);
                reads_queue.pop();

                if (batch.size() == 10000)
                {
                    break;
                }
            }
        }

        condition.notify_all();

        if (batch.size() > 0)
        {
            processLinesBatch(batch, all_kmers, output, k_size, threads, bin_size, bins);
            batch.clear();
        }

        {
            unique_lock<mutex> lock(mux);
            if (terminate_threads && reads_queue.size() == 0)
            {
                break;
            }
        }
    }
}

void io_thread(string &file_path)
{
    SeqReader reader(file_path);
    Seq seq;
    int count = 0;

    while (reader.get_seq(seq))
    {
        {
            unique_lock<mutex> lock(mux);
            condition.wait(lock, [] { return reads_queue.size() < 50000; });
            reads_queue.push(seq.data);
        }
        count++;
        if (count % 10000 == 0)
        {
            cout << "Loaded Reads " << count << "       \r" << flush;
        }
    }

    cout << endl;

    terminate_threads = true;
}

int main(int argc, char **argv)
{
    string input_path = argv[1];
    string output_path = argv[2];
    u_int64_t k_size = stoull(argv[3]);
    int bin_size = stoi(argv[4]);
    int bins = stoi(argv[5]);
    int threads = stoi(argv[6]);

    if (k_size > 31 || k_size < 7)
    {
        return -1;
    }

    terminate_threads = false;

    unordered_map<u_int64_t, atomic<u_int32_t>> kmer_index;
    
    {
        thread iot(io_thread_counter, ref(input_path));
        thread process(off_load_process_counter, ref(kmer_index), ref(k_size), ref(threads));

        iot.join();
        process.join();
    }

    cout << "INPUT FILE " << input_path << endl;
    cout << "OUTPUT FILE " << output_path << endl;
    cout << "K SIZE " << k_size << endl;
    cout << "BIN WIDTH " << bin_size << endl;
    cout << "BINS IN HIST " << bins << endl;
    cout << "THREADS " << threads << endl;

    terminate_threads = false;

    ofstream output(output_path, ios::out);

    {
        thread iot(io_thread, ref(input_path));
        thread process(off_load_process, ref(output_path), ref(kmer_index), ref(threads), ref(k_size), ref(bin_size), ref(bins));

        iot.join();
        process.join();
    }

    cout << "COMPLETED : Output at - " << output_path << endl;

    return 0;
}
