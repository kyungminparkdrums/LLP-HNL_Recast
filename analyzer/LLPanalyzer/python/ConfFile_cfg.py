import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

options = VarParsing.VarParsing('analysis')
options.parseArguments()

process = cms.Process("LLPanalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
                                  options.inputFiles
                                  #'file:LLP_HNL.root'
                )
                            )

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string(options.outputFile)
                                   )


process.demo = cms.EDAnalyzer('LLPanalyzer',
    electrons = cms.untracked.InputTag('slimmedElectrons'),
    muons = cms.untracked.InputTag('slimmedMuons'),
    trgResults = cms.untracked.InputTag("TriggerResults","","HLT"),
                              )

process.p = cms.Path(process.demo)

#process.dump=cms.EDAnalyzer('EventContentAnalyzer')

#process.p = cms.Path(process.demo*process.dump)

