import os

cfg = 'python/ConfFile_cfg.py'

'''
Input Files
'''
inputFiles = {'signal': {}, 'bkg': {}}

# Signal
#inputFiles['signal']['M-1_e_massiveAndCKM'] ='/store/mc/RunIISummer16MiniAODv3/HeavyNeutrino_trilepton_M-1_V-0_0949736805647_e_massiveAndCKM_LO_madgraph_pythia8/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3-v1/50000/686329BC-1BF7-E911-8EEE-008CFA1983BC.root'

inputFiles['signal']['signal'] = '/store/test/xrootd/T2_US_Purdue/store/mc/RunIISummer16MiniAODv2/HeavyNeutrino_trilepton_M-10_V-0p0001_2l_NLO_displaced/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/70000/10A42D2B-2274-E711-AA76-047D7B2E84EC.root'

# Bkg with xsec of O(100pb) or higher, except for TTJets_DiLept (87.315pb)
inputFiles['bkg']['DYJetsToLL_M-50'] = '/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/110000/005ED0EB-79F1-E611-B6DA-02163E011C2B.root'    # 6020.85 pb
inputFiles['bkg']['DYJetsToLL_M-10to50'] = '/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-10to50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/100000/023DEF48-55D2-E611-809C-E0071B7AC5D0.root'    # 18610 pb
inputFiles['bkg']['WJetsToLNu'] = '/store/mc/RunIISummer16MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/009CE684-45BB-E611-A261-001E67E6F8FA.root'    # 61334.9 pb
inputFiles['bkg']['TTJets_DiLept'] = '/store/mc/RunIISummer16MiniAODv2/TTJets_DiLept_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/1857B269-D0B6-E611-A679-6C3BE5B51238.root'    # 87.315 pb
inputFiles['bkg']['TTJets_SingleLeptFromT'] = '/store/mc/RunIISummer16MiniAODv2/TTJets_SingleLeptFromT_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/10B67D45-93BE-E611-A9C9-A0000420FE80.root'    # 182.175 pb
inputFiles['bkg']['TTJets_SingleLeptFromTbar'] = '/store/mc/RunIISummer16MiniAODv2/TTJets_SingleLeptFromTbar_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/04566AA8-BABD-E611-8E2D-0025905B85E8.root'    # 182.175 pb
inputFiles['bkg']['WGToLNuG'] = '/store/mc/RunIISummer16MiniAODv2/WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/120000/481D0A07-2CBB-E611-BF96-485B3919F14E.root'    # 405.271 pb
inputFiles['bkg']['ZGTo2LG'] = '/store/mc/RunIISummer16MiniAODv2/ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/100000/0C922DBB-7CD2-E611-A00E-0CC47A7EEE0E.root'    # 123.9 pb


'''
Output file and the command
'''
outputFile = '/uscms/home/kyungmip/nobackup/LLP/CMSSW_10_2_18/src/analyzer/LLPanalyzer/output/output.root'

sigOrBkg = 'bkg'
process = 'DYJetsToLL_M-10to50'

cmd = 'cmsRun {} inputFiles={} outputFile={}'.format(cfg, inputFiles[sigOrBkg][process], outputFile)

print(cmd)
os.system(cmd)
