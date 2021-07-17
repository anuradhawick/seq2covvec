# seq2covvec

Coverage vector computation algorithm presented in [MetaBCC-LR](https://doi.org/10.1093/bioinformatics/btaa441).
Computations are much faster now due to several improvements we have done. More flexibility for vectors are also included.

### Build

```
sh build.sh
```

### Check help

```
./seq2covvec -h

usage: seq2covvec.py [-h] --reads-path READS_PATH [--k-size [11-31]]
                     [--bin-size BIN_SIZE] [--bin-count BIN_COUNT]
                     [--threads THREADS] --output OUTPUT

Convert sequences into coverage vectors. Supports k-mer sizes from 11-31.

optional arguments:
  -h, --help            show this help message and exit
  --reads-path READS_PATH, -r READS_PATH
                        Reads path for binning
  --k-size [11-31], -k [11-31]
                        K size for the coverage histogram.
  --bin-size BIN_SIZE, -bs BIN_SIZE
                        Bin size for the coverage histogram.
  --bin-count BIN_COUNT, -bc BIN_COUNT
                        Number of bins for the coverage histogram.
  --threads THREADS, -t THREADS
                        Thread count for computations
  --output OUTPUT, -o OUTPUT
                        Output file name
```
