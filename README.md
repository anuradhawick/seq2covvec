# seq2covvec: Coverage vector generation for binning long reads datasets

Coverage vector computation algorithm presented in [MetaBCC-LR](https://doi.org/10.1093/bioinformatics/btaa441).
Computations are much faster now due to several improvements we have done. More flexibility for vectors are also included.

Supports k-mer sizes from 11 to 31. Higher k values demands larger memory. 

> Future improvements to follow include; faster buffered hashing of k-mers, k-mer count thresholds to avoid too abundant, or too scarce k-mers.

For PacBio HiFi and Oxford Nanopore Q20+ reads, try k values above 19 else use 15 or lower.

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

### Citation

```bibtex
@article{10.1093/bioinformatics/btaa441,
    author = {Wickramarachchi, Anuradha and Mallawaarachchi, Vijini and Rajan, Vaibhav and Lin, Yu},
    title = "{MetaBCC-LR: metagenomics binning by coverage and composition for long reads}",
    journal = {Bioinformatics},
    volume = {36},
    number = {Supplement_1},
    pages = {i3-i11},
    year = {2020},
    month = {07},
    abstract = "{Metagenomics studies have provided key insights into the composition and structure of microbial communities found in different environments. Among the techniques used to analyse metagenomic data, binning is considered a crucial step to characterize the different species of micro-organisms present. The use of short-read data in most binning tools poses several limitations, such as insufficient species-specific signal, and the emergence of long-read sequencing technologies offers us opportunities to surmount them. However, most current metagenomic binning tools have been developed for short reads. The few tools that can process long reads either do not scale with increasing input size or require a database with reference genomes that are often unknown. In this article, we present MetaBCC-LR, a scalable reference-free binning method which clusters long reads directly based on their k-mer coverage histograms and oligonucleotide composition.We evaluate MetaBCC-LR on multiple simulated and real metagenomic long-read datasets with varying coverages and error rates. Our experiments demonstrate that MetaBCC-LR substantially outperforms state-of-the-art reference-free binning tools, achieving ∼13\\% improvement in F1-score and ∼30\\% improvement in ARI compared to the best previous tools. Moreover, we show that using MetaBCC-LR before long-read assembly helps to enhance the assembly quality while significantly reducing the assembly cost in terms of time and memory usage. The efficiency and accuracy of MetaBCC-LR pave the way for more effective long-read-based metagenomics analyses to support a wide range of applications.The source code is freely available at: https://github.com/anuradhawick/MetaBCC-LR.Supplementary data are available at Bioinformatics online.}",
    issn = {1367-4803},
    doi = {10.1093/bioinformatics/btaa441},
    url = {https://doi.org/10.1093/bioinformatics/btaa441},
    eprint = {https://academic.oup.com/bioinformatics/article-pdf/36/Supplement\_1/i3/33488763/btaa441.pdf},
}
```
