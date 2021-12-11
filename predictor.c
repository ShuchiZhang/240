//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

uint32_t BHR;
int *predictTable;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
    predictTable = (int *)malloc((1<<ghistoryBits) * sizeof(int));
    for (int i = 0; i < (1<<ghistoryBits); i++) predictTable[i] = WN;
    BHR = 0;
}
 
// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t 
makeGsharePrediction(uint32_t pc){
  uint32_t index = (BHR ^ pc)&((1 << ghistoryBits)-1);
  uint8_t pred = predictTable[index];
  return (pred == WT || pred == ST)?TAKEN:NOTTAKEN;
}

uint8_t
make_prediction(uint32_t pc)
{

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return makeGsharePrediction(pc);
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
uint8_t
newVal(uint8_t pred, uint8_t outcome) {
  if(outcome == NOTTAKEN){
    if(pred == WN) return SN;
    else if(pred == ST) return WT;
    else if(pred == WT) return WN;
  }
  else{
    if(pred == WT) return ST;
    else if(pred == WN) return WT;
    else if(pred == SN) return WN;
  }
  return pred;
}

void
train_gsahre_predictor(uint32_t pc, uint8_t outcome)
{ 
  uint32_t index = (BHR ^ pc)&((1 << ghistoryBits)-1);
  uint8_t pred = predictTable[index];
  predictTable[index] = newVal(pred, outcome);
  BHR = (BHR<<1 | outcome) & ((1<<ghistoryBits) - 1);
}

void
train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType) {
    case GSHARE:
      train_gsahre_predictor(pc, outcome);
      break;
    case TOURNAMENT:
    case CUSTOM:
    default:
    break;
  }
      
}
