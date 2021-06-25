#!/usr/bin/python
import os
import sys
import argparse


def check_proc(ret, name=""):
    if ret != 0:
        if name != "":
            print(f"Error in step: {name}")
        print("Failed due to an error. Please check the log. Good Bye!")
        sys.exit(ret)

def run_15mer_vecs(reads_path, output, bin_size, bin_count, threads):
    cmd = f""""{os.path.dirname(__file__)}/bin/search-15mers" "{reads_path}" "{output}" {bin_size} {bin_count} {threads}"""
    print("CMD::" + cmd)
    o = os.system(cmd)
    check_proc(o, "Counting 15-mer profiles")

def main():
    parser = argparse.ArgumentParser(description="""Convert sequences into coverage vectors. Only supports 15-mers at the moment.""")

    parser.add_argument('--reads-path', '-r',
                        help="Reads path for binning",
                        type=str,
                        required=True)
    parser.add_argument('--bin-size', '-bs',
                        help="Bin size for the coverage histogram.",
                        type=int,
                        required=False,
                        default=10)
    parser.add_argument('--bin-count', '-bc',
                        help="Number of bins for the coverage histogram.",
                        type=int,
                        required=False,
                        default=32)
    parser.add_argument('--threads', '-t',
                        help="Thread count for computations",
                        type=int,
                        default=8,
                        required=False)
    parser.add_argument('--output', '-o',
                        help="Output directory", type=str, required=True)

    # command line args
    args = parser.parse_args()

    reads_path = args.reads_path
    threads = args.threads
    bin_size = args.bin_size
    bin_count = args.bin_count
    output = args.output

    run_15mer_vecs(reads_path, output, bin_size, bin_count, threads)

    print("Seq2CovVec Complete!")

if __name__ == '__main__':
    main()