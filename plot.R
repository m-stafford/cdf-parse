#!/usr/bin/Rscript --vanilla --default-packages=utils,scatterplot3d,graphics,grDevices
mydata <- read.csv("data.csv")
args <- commandArgs(trailingOnly = TRUE)
plotname <- args[1]
jpeg(plotname, width = 900, height = 700)
suppressWarnings(plot(mydata$time,mydata$bz, type = "line",ylab="B, z-dir, GSE coord, nT", xlab="Time from UNIX Epoch in seconds"))

