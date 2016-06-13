
/***********************************************************************
*  This code is part of Coliop3
*
*  Coliop3 and CMPL are projects of the Institute for Operations Research
*  and Business Management at the Martin Luther University Halle-Wittenberg 
*  and the University of Applied Sciences Wildau. 
*  Please visit the project homepage <www.coliop.org>
*
*  Coliop3 is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Coliop3 is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Coliop. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

package coliop3;


import com.apple.eawt.*;
import javax.swing.JFrame;
import javax.swing.JOptionPane;

/**
 *
 * @author mike
 */
public class ColiopMacApplicationListener extends ApplicationAdapter {

    
    private JFrame parent = null; // für die Dialoge 
    private final Coliop3 clpUI;

    public ColiopMacApplicationListener(JFrame parent, Coliop3 clpUI) { 
        this.parent = parent; 
        this.clpUI = clpUI;

    }

   
        
    @Override
    public void handlePreferences(ApplicationEvent event) {
        
        new ColiopOptions(parent).setVisible(true);
        clpUI.clp.loadOptions();
        clpUI.clpSetFont();
        clpUI.setStatusBar();
        event.setHandled(true); 
        
    }    
        
    @Override
        public void handleAbout(ApplicationEvent event) { 
            new ColiopAbout(parent).setVisible(true);
            /* Dialog aufbauen und anzeigen */ 
            event.setHandled(true); // Event wurde behandelt
        } 
        
              
    @Override
        public void handleQuit(ApplicationEvent event) { 
            event.setHandled(true); 
            
           if (clpUI.clp.probChanged) {
               int ret = JOptionPane.showConfirmDialog(null,
                        "Save your problem?\n" + clpUI.clp.name, "unsaved problem",
                        JOptionPane.YES_NO_OPTION);

                if (ret == JOptionPane.YES_OPTION) {
                    clpUI.clpSaveProblem(0);
                }
            }

            if (clpUI.activeSolverHandler!=null && clpUI.activeSolverHandler.isAlive()) {
                    JOptionPane.showMessageDialog(null,
                        "Solver is running - Please cancel Solver before closing Coliop3",
                        "Solver is running", JOptionPane.WARNING_MESSAGE);
                    event.setHandled(false); 
            } else 
                System.exit(0);
   
            
        } 
    
        
}
    
    
    
    

