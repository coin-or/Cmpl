# SolverStudio CMPL Processor
# v1.0.0, 30 August 2014
# v1.1.0, 14 May 2016
# Mike Steglich


import clr
clr.AddReference('System.Windows.Forms')
clr.AddReference('System.Drawing')

import webbrowser
import os
import sys
import cStringIO

from System.Windows.Forms import  Form, DockStyle, Panel, Button, TextBox, FormBorderStyle,MessageBox, ScrollBars, AnchorStyles
from System.Drawing import Font

import version


#************* pyCmpl and cmplserver includes ****************
cmplPath=SolverStudio.LanguageDirectory()
#print "cmplPath="+cmplPath
os.environ.update({'CMPLPATH':cmplPath })
#************* end pyCmpl and cmplserver includes ************

#************* writeCmplOptFile ******************************
def writeCmplOptFile():
  optFile=SolverStudio.LanguageDirectory()+os.sep+"bin"+os.sep+"cmpl.opt"
   
  newOpt=""
  try:
    with open(optFile, 'r') as f:
      for line in f:
        
        ret=line.split("=")
                              
        if ret[0].strip().lower() == "cbcpath":
          tmpTxt = SolverStudio.GetSolversPaths()[1]
          tmpTxt="CbcPath = " + tmpTxt[2:]
          newOpt+=tmpTxt.replace('\\','/') + "/\n"
          continue
        
        if ret[0].strip().lower() == "glpkpath":
          tmpTxt = SolverStudio.GetGLPSolPath()
          tmpTxt="GlpkPath = " + tmpTxt[2:]
          tmpTxt=tmpTxt.replace("glpsol.exe","")
          newOpt+=tmpTxt.replace('\\','/') + "\n"
          continue
        
        newOpt+=line
            
    f.close()
    
    with open(optFile, 'w') as f:
      f.write(newOpt)
    f.close()
      
      
  except Exception,e :
      pass
      #raise Exception ("Can't read or write the CMPL option file >" + str(e))
      #sys.exit()
#************* end writeCmplOptFile **************************


#*********** Menue functions ********************
def OpenCMPLMailingList(key,e):
  webbrowser.open("""http://list.coin-or.org/mailman/listinfo/Cmpl""")
  
def OpenCMPLWebSite(key,e):
  webbrowser.open("""http://www.coliop.org""")
  
def OpenCMPLpdf(key,e):
  oldDirectory = SolverStudio.ChangeToLanguageDirectory()
  os.startfile("doc\CMPL.pdf")  
  SolverStudio.SetCurrentDirectory(oldDirectory)

def OpenCOPYING(key,e):
  oldDirectory = SolverStudio.ChangeToLanguageDirectory()
  showFile=ShowTextFile("LICENSE.txt")
  showFile.ShowDialog()
  SolverStudio.SetCurrentDirectory(oldDirectory)

def OpenREADME(key,e):
  oldDirectory = SolverStudio.ChangeToLanguageDirectory()
  showFile=ShowTextFile("README.txt")
  showFile.ShowDialog()
  SolverStudio.SetCurrentDirectory(oldDirectory)
  
def OpenSolFile(key, e):
  OpenFile("sol")
  
def OpenMatFile(key, e):
   OpenFile("mat")
   
def OpenDataFile(key, e):
   OpenFile("data")

def OpenFile(mode):
  workingDir = SolverStudio.WorkingDirectory()
  if mode=="sol":
    FileName = "model.sol"
  elif mode=="mat":
    FileName = "model.mat"
  elif mode=="data":
    FileName = "model.cdat"  
  
  dataFilePath = os.path.realpath(os.path.normpath(os.path.join(workingDir, FileName)))
  
  showFile=ShowTextFile(dataFilePath)
  showFile.ShowDialog()
     
def About(key,e):
  MessageBox.Show("SolverStudio CMPL Processor:\n"+version.versionString + "\nMike Steglich" )
  
#*********** end menue functions *****************
 
#*********** ShowTextFile ***************	
class ShowTextFile(Form):
  
    def __init__(self, fName):
      
          fContent=cStringIO.StringIO()
          try:
             with open(fName, 'r') as f:
              for line in f:
                fContent.write(line+ "\r\n")
                #print line
              f.close()  
          except Exception,e :
            fContent.write("No content - Please solve before obtaining the solution/matrix/data file")
         
          self.Text = "SolverStudio CMPL Viewer: "+fName
          self.FormBorderStyle = FormBorderStyle.Sizable # SizableToolWindow # FixedDialog    
          self.Height=500
          self.Width = 600
          
          self.Content = TextBox()
          self.Content.Text=fContent.getvalue()
          self.Content.ReadOnly=True
          self.Content.SelectionStart = 0
          
          self.Content.Multiline = True
          self.Content.WordWrap = False
          self.Content.ScrollBars = ScrollBars.Both
          self.Content.Dock = DockStyle.Fill
          self.Content.Font=Font("Courier New",10)
          
          
          self.bOK=Button()
          self.bOK.Text = "OK"
          self.bOK.Anchor =  (AnchorStyles.Bottom | AnchorStyles.Right);
          # self.bOK.Dock = DockStyle.Right
          # self.bOK.Width = 75 AJM: Go with the default height/width to be more Windows-like
          # self.bOK.Height = 25
          self.bOK.Click += self.OK
          
          self.AcceptButton = self.bOK
          
          self.buttonPanel=Panel()
          self.buttonPanel.Dock=DockStyle.Bottom
          self.buttonPanel.Height=35
          self.buttonPanel.Controls.Add(self.bOK)
          
          self.bOK.Left = self.buttonPanel.Width - self.bOK.Width - 20 # AJM: Position button nicely
          self.bOK.Top = 7 # AJM: Position button nicely
         
          self.Controls.Add(self.Content)
          self.Controls.Add(self.buttonPanel)
        
          self.CenterToScreen()
          
    def OK(self,key,e):
      self.Close()
#*********** end ShowTextFile ************* 


#*********** OpenCmplServerDialog *********
def OpenCmplServerDialog(key, e):
  
  #workingDir = SolverStudio.WorkingDirectory()
  oldDirectory = SolverStudio.ChangeToWorkingDirectory() # This is the temp directory
  cmdStr = cmplPath+os.sep+"cmplServer"+os.sep+"cmplServerUi.py"
  try:
      ipy =SolverStudio.GetSolverStudioIPyWPath(mustExist=True) # Will throw an exception if ipy not found
      ant=SolverStudio.StartEXE(ipy, cmdStr , addExeDirectoryToPath=False, addSolverPath=True, addAtStartOfPath=True)
  finally:
      SolverStudio.SetCurrentDirectory(oldDirectory)

#*********** end OpenCmplServerDialog ******   
  
 
  

#*********** Initialise ********************
# Return s as a string suitable for indexing, which means tuples are listed items in []
def Initialise():
  
  #writeCmplOptFile()
  
  Menu.Add( "View Last Solution ",OpenSolFile)
  #Menu.Add( "View Last Generated Matrix ",OpenMatFile)
  Menu.Add( "View Last Generated Data File ",OpenDataFile)
  Menu.AddSeparator()
  Menu.Add( "Open CMPL Pdf Manual",OpenCMPLpdf)
  Menu.Add( "Open CMPL Web Page",OpenCMPLWebSite)
  Menu.Add( "Open CMPL Mailing List",OpenCMPLMailingList)

  Menu.AddSeparator()
  Menu.Add( "Open CMPLServer",OpenCmplServerDialog)

  Menu.AddSeparator()
  Menu.Add( "Open README file",OpenREADME) 
  Menu.Add( "Open LICENSE file",OpenCOPYING)
  Menu.AddSeparator()
  Menu.Add( "About CMPL Processor",About)
#*********** end Initialise *****************
  
