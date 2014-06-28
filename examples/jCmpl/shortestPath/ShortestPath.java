/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package shortestpath;

import jCMPL.*;


public class ShortestPath {
    public static void main(String[] args) throws CmplException {
        
        try {
            Cmpl m = new Cmpl("shortest-path.cmpl");

            CmplSet routes = new CmplSet("A",2);
            int[][] arcs = {{1,2},{1,4},{2,1},{2,3},{2,4},{2,5},
				 {3,2},{3,5},{4,1},{4,2},{4,5},{4,6},
				 {5,2},{5,3},{5,4},{5,6},{5,7},
				 {6,4},{6,5},{6,7},{7,5},{7,6}};
            routes.setValues(arcs);
        
            CmplSet nodes = new CmplSet("V");
            nodes.setValues(1,7);
            
            CmplParameter c = new CmplParameter("c", routes);
            Integer[] cArr = {7,5,7,8,9,7,8,5,5,9,15,6,7,5,15,8,9,6,8,11,9,11};
            c.setValues(cArr);
            
            CmplParameter sNode = new CmplParameter("s");
            sNode.setValues(1);
            
            CmplParameter tNode = new CmplParameter("t");
            tNode.setValues(7);
            
            m.setSets(routes, nodes);
            m.setParameters(c, sNode, tNode);
            
            m.setOption("%display nonZeros");
            m.connect("http://127.0.0.1:8008");
           // m.debug(true);
            m.solve();


            if (m.solverStatus() == Cmpl.SOLVER_OK) {
             
                System.out.println("Objective value     :" + m.solution().value() );

                for (CmplSolElement v : m.solution().variables()) {
                    System.out.println( v.name() + " " + v.activity() );
                }
          

            } else {
                System.out.println("Solver failed " + m.solver() + " " + m.solverMessage());
            }

                 
        } catch (CmplException e) {

            System.out.println(e);

        } 
    }
}
