context("Basic read")

ftest1 <- system.file("extdata", "ocfiles", "S2008001.L3b_DAY_CHL.main", package = "roc")
ftest2 <- system.file("extdata", "ocfiles", "S2010006.L3b_DAY_RRS.main", package = "roc")

inqnames <- c("SEAGrid", "BinList", "BinIndex")
test_that("Vdata names read successfully", {
  vinfo <- vdatainfo(ftest1)
  expect_that(vinfo, is_a("list"))
  expect_that(length(vinfo), equals(4L))
  expect_that(all(inqnames %in% names(vinfo)), is_true())
})
test_that("file read is successful", {
  vinfo <- vdatainfo(ftest1)
  expect_that(file.exists(ftest1), is_true()) 
  expect_that(rrshdf4:::binlist(ftest1, setdiff(names(vinfo), inqnames), TRUE),is_a("list"))
  expect_that(names(rrshdf4:::binlist(ftest2, setdiff(names(vdatainfo(ftest2)), inqnames), TRUE))[21L], equals("Rrs_670_ssq"))
 })

