rrshdf4
=====

R package to read Level-3 bin files. 

These bins
are a sparse, non-rectangular, grid on a Sinusoidal projection, see 

http://oceandata.sci.gsfc.nasa.gov/

http://oceancolor.gsfc.nasa.gov/SeaWiFS/TECH_REPORTS/PreLPDF/PreLVol32.pdf

Package is built using roxygen2 and Rcpp. Only tested on Linux for now - help welcome to port to Windows. 

Limitations
====
When the x00, x01, etc files are required they *must* be present next to the main file. This package does some simplistic tests for this, but it's not finished. If they are not present, the relevant variables will be populated with zero values but this otherwise does not affect functionality.  

Basic usage
====

Get a Level-3 bin file  (this one is 15 Mb) of ocean colour, and read all bins. 

```{r}
f <- "http://oceandata.sci.gsfc.nasa.gov/cgi/getfile/A2014208.L3b_DAY_KD490.main.bz2"
download.file(f, basename(f), mode = "wb")
library(rrshdf4)
system(sprintf("bunzip2 %s", basename(f)))
x <- binlist(gsub(".bz2", "", basename(f)))

```

Default behaviour is to read both sum/ssq of *all* variables. You can limit to just some variables with the *vname* argument. (This can be important for the RRS files which contain many Remote Sensing Reflectance variables. 

TODO
====

- Ability to read sum and/or ssq for individual variables. 
- Distinction between grid index data and variables without hardcoded names. 


Dependencies
====

These are the complete steps to install from a basic Ubuntu system.  

```{bash}
## 1) Update and install R

## using Nectar image "NeCTAR Ubuntu 14.04 (Trusty) amd64"
## key for apt-get update, see http://cran.r-project.org/bin/linux/ubuntu/README
sudo chown ubuntu /etc/apt/sources.list
sudo echo 'deb http://cran.csiro.au/bin/linux/ubuntu trusty/' >> /etc/apt/sources.list
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9
sudo apt-get update && sudo apt-get upgrade -y
sudo apt-get install -y r-base r-base-dev 

## 2) Install R package dependencies
## brew, digest, Rcpp stringr, and testthat packages
## lib = "/usr/lib/R/site-library"
sudo Rscript -e 'x <- .libPaths();install.packages(c("roxygen2", "testthat", "Rcpp"), lib = x[length(x)-1], repos = "http://cran.csiro.au")'

## 3) Install 3rd party HDF 

## not sure if both required?
sudo apt-get install -y libhdf4-dev
sudo apt-get install -y libhdf4g-dev
sudo apt-get install git -y

## 4) Get the package source and install
git clone https://github.com/AustralianAntarcticDivision/rrshdf4.git

### 4a) roxygenize
### Rscript -e 'roxygen2::roxygenize("rrshdf4")'
Rscript -e 'roxygen2::roxygenize("rrshdf4")'

### 4b) Rcpp attributes (wraps the C++ source for R functions and doc)
Rscript -e 'Rcpp::compileAttributes("rrshdf4")'

### 4c) build and install
## check where your hdf.h is, ensure this corresponds to rrshdf4/src/Makevars and
export CPATH=/usr/include/hdf
R CMD build rrshdf4

echo 'CPATH=/usr/include/hdf' >> .Renviron


=======
##Rscript -e 'roxygen2::roxygenize("rrshdf4")'
## not sure if this is a bug in roxygen2, but ensure methods is loaded first
Rscript -e 'library(methods);roxygen2::roxygenize("rrshdf4")'
## check where your hdf.h is, ensure this corresponds to rrshdf4/src/Makevars and
export CPATH=/usr/include/hdf
R CMD build rrshdf4
sudo R CMD INSTALL rrshdf4_someversion.tar.gz --library=/usr/local/lib/R/site-library


```

