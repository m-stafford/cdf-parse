#!/usr/bin/Rscript --vanilla --default-packages=utils,fields,scatterplot3d,graphics,grDevices
mydata <- read.csv("data-thc-esa-en.csv")
args <- commandArgs(trailingOnly = TRUE)
plotname <- args[1]
jpeg(plotname, width = 900, height = 700)
suppressWarnings(image.plot(mydata))

