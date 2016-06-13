# SolverStudio pyCMPL Processor
# v1.0.0, March 2015
# Mike Steglich


import clr
clr.AddReference('System.Windows.Forms')
clr.AddReference('System.Drawing')

import webbrowser
import os
import sys

from System.Windows.Forms import  Form, DockStyle, Panel, Button, TextBox, FormBorderStyle,MessageBox, ScrollBars, AnchorStyles
from System.Drawing import Font

import version


#************* pyCmpl and cmplserver includes ****************
cmplPath=SolverStudio.LanguageDirectory().replace("PyCMPL","CMPL")
#print "cmplPath="+cmplPath
os.environ.update({'CMPLPATH':cmplPath })
#************* end pyCmpl and cmplserver includes ************

#*********** Menue functions ********************
def OpenCMPLMailingList(key,e):
  webbrowser.open("""http://list.coin-or.org/mailman/listinfo/Cmpl""")
  
def OpenCMPLWebSite(key,e):
  webbrowser.open("""http://www.coliop.org""")
  
def OpenCMPLpdf(key,e):
  fName = cmplPath+os.sep+"doc"+os.sep+"CMPL.pdf"
  os.startfile(fName)  


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
  
def About(key,e):
  MessageBox.Show("SolverStudio CMPL Processor:\n"+version.versionString + "\nMike Steglich" )
  
#*********** end menue functions *****************
 
#*********** ShowTextFile ***************	
class ShowTextFile(Form):
  
    def __init__(self, fName):
      
          fContent=""
          try:
             with open(fName, 'r') as f:
              for line in f: 
                fContent += line + "\r\n"
                #print line
              f.close()  
          except Exception,e :
            fContent="No content "
         
          self.Text = "SolverStudio pyCMPL Viewer: "+fName
          self.FormBorderStyle = FormBorderStyle.Sizable # SizableToolWindow # FixedDialog    
          self.Height=500
          self.Width = 600
          
          self.Content = TextBox()
          self.Content.Text=fContent
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
  
  #Menu.Add( "View Last Solution ",OpenSolFile)
  #Menu.Add( "View Last Generated Matrix ",OpenMatFile)
  #Menu.Add( "View Last Generated Data File ",OpenDataFile)
  #Menu.AddSeparator()
  Menu.Add( "Open pyCmpl/CMPL Pdf Manual",OpenCMPLpdf)
  Menu.Add( "Open CMPL Web Page",OpenCMPLWebSite)
  Menu.Add( "Open CMPL Mailing List",OpenCMPLMailingList)

  Menu.AddSeparator()
  Menu.Add( "Open CMPLServer",OpenCmplServerDialog)

  Menu.AddSeparator()
  Menu.Add( "Open README file",OpenREADME) 
  Menu.Add( "Open LICENSE file",OpenCOPYING)
  Menu.AddSeparator()
  Menu.Add( "About pyCMPL Processor",About)
#*********** end Initialise *****************
  
