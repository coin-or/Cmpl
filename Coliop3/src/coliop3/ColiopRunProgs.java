/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  cmplFrame and CMPL are projects of the Institute for Operations Research
 *  and Business Management at the Martin Luther University Halle-Wittenberg
 *  and the University of Applied Sciences Wildau.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  cmplCore is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cmplFrame is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Coliop. If not, see <http://www.gnu.org/licenses/>.
 *
 *  @author Mike Steglich - www.coliop.org
 *
 ***********************************************************************/


package coliop3;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PipedOutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;



public class ColiopRunProgs extends Thread  {

    private ArrayList progSolverCodes;
    private ArrayList progCmplCodes;
    private Process p;
    private boolean runCanceled=false;

    private PrintWriter cmplOutputStream;


   


    ColiopRunProgs( PipedOutputStream outCmplPipe ) {
        cmplOutputStream  = new PrintWriter(outCmplPipe, true );
        
    }

 
    @Override
    public void interrupt(){
       stopActions();
       p.destroy();
       runCanceled=true;
    }


    @Override
    public void run()  {

        try {
            ProcessBuilder launcher = new ProcessBuilder(progCmplCodes);
            launcher.redirectErrorStream(true);
            p = launcher.start();
              
            BufferedReader output = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line;
           
            while ((line = output.readLine()) != null) {
                    cmplOutputStream.println(line);
            }

            if (!runCanceled) {
                p.waitFor();
            }      
      
            cmplOutputStream.close();
            
        } catch (IOException e) {
                cmplOutputStream.println("Runtime Error " + e );
     
        } catch (InterruptedException e) {
            cmplOutputStream.println("Runtime Error " + e );
        }

   
    }

    public void setModel(ArrayList cmplCodes) {
        progCmplCodes=cmplCodes;
    }

    public int exitValue() {
        return p.exitValue();
    }
   
    
    public boolean isRunning() {
        return this.isAlive();  
    }

    private void stopActions() {
        cmplOutputStream.println("CMPL - Canceled by user");
        cmplOutputStream.close();
    
    }

   

}
