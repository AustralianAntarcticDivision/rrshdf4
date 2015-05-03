// source taken with permission from Norman Kuring (NASA)
// http://oceancolor.gsfc.nasa.gov/DOCS/OCSSW/l3bindump_8c_source.html
// and from smigen.cpp (NASA, Seadas)
// http://oceancolor.gsfc.nasa.gov/DOCS/OCSSW/smigen_8cpp_source.html

#include <math.h>
#include "hdf.h"

#include <Rcpp.h>
using namespace Rcpp;

#define BLIST_FIELDS "bin_num,nobs,nscenes,time_rec,weights,sel_cat,flags_set"




// [[Rcpp::export]]
List binlist(CharacterVector filename, CharacterVector vname, LogicalVector bins) {
  // file name
  std::string fname = Rcpp::as<std::string>(filename);
  
  // collect Rcpp vectors in a list
  List z  = List::create() ;
  
  
  /* counters */
  int i, jvar, nvar, numrecs, *binnums = NULL;
  /* file refs */
  int file_id, vdata_ref, vdata_id,pvd_id;
  
  // updated MDS 2014-08-09
  //const char * c_filename = fname.c_str();
  file_id = Hopen(fname.c_str(), DFACC_READ, 0);
  /* Initialize the Vdata interface. */
  Vstart(file_id);
  
  
  vdata_ref = VSfind(file_id,"BinIndex");
  vdata_id = VSattach(file_id, vdata_ref, "r");
  /* Find out how many rows */
  int numrs = VSelts(vdata_id);
  //IntegerVector xl;
  z["NUMROWS"] = numrs;
  
  // finish up, close the file
  VSdetach(vdata_id);
  
  
  
  /* Open the "BinList" Vdata. */
  vdata_ref = VSfind(file_id,"BinList");
  
  vdata_id = VSattach(file_id, vdata_ref, "r");
  /* Find out how many bins are stored in this file. */
  numrecs = VSelts(vdata_id);
  
  if (bins[0]) {
    
    /* Rcpp structures for shared binlist */
    IntegerVector bin(numrecs);
    IntegerVector nobservations(numrecs);
    IntegerVector nsc(numrecs);
    NumericVector wghts(numrecs);
    
    /* Set up to read the fields in the BinList Vdata records. */
    VSsetfields(vdata_id,BLIST_FIELDS);
    
    int * bin_num = new int[numrecs];
    int * flags_set = new int[numrecs];
    short * nobs = new short[numrecs];
    short * nscenes = new short[numrecs];
    short * time_rec = new short[numrecs];
    
    float32 * weights = new float32[numrecs];
    uint8 *  sel_cat = new uint8[numrecs];
    VOIDP    bufptrs[] = {
      &bin_num[0],&nobs[0],&nscenes[0],&time_rec[0],&weights[0],&sel_cat[0],&flags_set[0]
    };
    
    int bibuf = 19 * numrecs;
    uint8 * blistrec = new uint8[bibuf];
    
    // pretty sure this is not needed
    //VSseek(vdata_id,i); 
    VSread(vdata_id,blistrec,numrecs,FULL_INTERLACE); 
    /*
    VSfpack() sets the global bin_num variable (and others)
    via the bufptrs pointer array.
    */
    
    VSfpack(vdata_id,_HDF_VSUNPACK,BLIST_FIELDS,blistrec, bibuf, numrecs,NULL,bufptrs); 
    
    // separate loops for vdatat_id and pvd_id, because they seek to different places
    for (i = 0; i < numrecs; i++ ) {
      // populate the Rcpp vectors
      bin[i] = bin_num[i];
      nobservations[i] = nobs[i];
      nsc[i] = nscenes[i];
      wghts[i] = weights[i];
      
    }   
    delete[] bin_num;
    delete[] flags_set;
    delete[] nobs;
    delete[] nscenes;
    delete[] time_rec;
    delete[] weights;
    delete[] sel_cat;
    delete[] blistrec;

    
    //  
    z["bin_num"] = bin;
    z["nobs"] = nobservations;
    z["nscenes"] = nsc;
    z["weights"] = wghts;
    
  } 
  // loop over all parameters, get the sum/ssq for each
  nvar = vname.size();
  for (jvar = 0; jvar < nvar; jvar++) {
    // reset the sum/ssq Rcpp vectors each time
    NumericVector parsum(numrecs);
    NumericVector parssq(numrecs);
    
    std::string vstrname  = Rcpp::as<std::string>(vname[jvar]);   
    
    int PREC_SIZE = 8 * numrecs;
    //int * bin_num = new int[numrecs];
    uint8 * paramrec = new uint8[PREC_SIZE];
    char *param_fields;
    float32 * summ = new float32[numrecs];
    float32 * sum_sq = new float32[numrecs];
    VOIDP    paramptrs[] = {&summ[0], &sum_sq[0]};
    
    
    /* Open the parameter-specific Vdata. */
    // updated MDS 2014-08-09
    //const char * PARAM = vstrname.c_str();
    vdata_ref = VSfind(file_id,vstrname.c_str());
    pvd_id = VSattach(file_id, vdata_ref, "r");
    /* Set up to read the fields in the parameter-specific Vdata records. */
    {
      int len;
      //len = 2*strlen(PARAM) + strlen("_sum,") + strlen("_sum_sq") + 1;
      len = 2*vstrname.size() + strlen("_sum,") + strlen("_sum_sq") + 1;
      param_fields = (char *)malloc(len);
      strcpy(param_fields,vstrname.c_str());
      strcat(param_fields,"_sum,");
      strcat(param_fields,vstrname.c_str());
      strcat(param_fields,"_sum_sq");
    }
    
    VSsetfields(pvd_id,param_fields); 
    
    /*
    Read the sum and sum-of-squares for the
    the specified parameter for this bin.
    */
    
    //VSseek(pvd_id,i); 
    VSread(pvd_id,paramrec, numrecs, FULL_INTERLACE); 
    /*
    VSfpack() sets the global sum and sum_sq variables
    via the paramptrs pointer array.
    */
    VSfpack(pvd_id,_HDF_VSUNPACK,param_fields,paramrec,PREC_SIZE, numrecs, NULL,paramptrs); 
    
    
    for (i = 0; i < numrecs; i++ ) { 
      parsum[i] = summ[i];
      parssq[i] = sum_sq[i];
      
    }
    // this has to come *after* the Rcpp loop just above
    free(param_fields);
    
    delete[] paramrec;
    delete[] summ;
    delete[] sum_sq;
    
    //removal of recursive list, all at one level
    //List z2  = List::create();
    String nm1 = vstrname;
    nm1 += "_sum";
    String nm2 = vstrname;
    nm2 += "_ssq";
    z[nm1] = parsum;
    z[nm2] = parssq; 
    
    VSdetach(pvd_id);
    
  }
  
  // finish up, close the file
  VSdetach(vdata_id); 
  Vend(file_id);
  Hclose(file_id); 
  
  free(binnums);
  
  return z;
}

