/* ****************************************************************************
 * This code is part of jCMPL
 *
 * Copyright (C) 2013 Mike Steglich / B. Knie Technical University of Applied
 * Sciences Wildau, Germany
 *
 * pyCMPL is a project of the Technical University of Applied Sciences Wildau
 * and the Institute for Operations Research and Business Management at the
 * Martin Luther University Halle-Wittenberg.
 *
 * Please visit the project homepage <http://www.coliop.org>
 *
 * pyCMPL is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * pyCMPL is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public # License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

import jCMPL.*;

import java.util.ArrayList;

public class Transportation {

    public static void main(String[] args) throws CmplException {
        try {
            Cmpl m = new Cmpl("transportation.cmpl");

            CmplSet routes = new CmplSet("routes", 2);
            int[][] arcs = {{1, 1}, {1, 2}, {1, 4}, {2, 2}, {2, 3}, {2, 4}, {3, 1}, {3, 3}};
            routes.setValues(arcs);

            CmplSet plants = new CmplSet("plants");
            plants.setValues(1, 3);

            CmplSet centers = new CmplSet("centers");
            centers.setValues(1, 1, 4);

            CmplParameter costs = new CmplParameter("c", routes);
            Integer[] costArr = {3, 2, 6, 5, 2, 3, 2, 4};
            costs.setValues(costArr);

            CmplParameter s = new CmplParameter("s", plants);
            ArrayList sList = new ArrayList();
            sList.add(5000);
            sList.add(6000);
            sList.add(2500);
            s.setValues(sList);

            CmplParameter d = new CmplParameter("d", centers);
            int[] dArr = {6000, 4000, 2000, 2500};
            d.setValues(dArr);

            m.setSets(routes, plants, centers);
            m.setParameters(costs, s, d);

            m.setOutput(true);
            m.setOption("%arg -solver cbc");

            //start CmplServer first with cmplServer -start
            //m.connect("http://127.0.0.1:8008");
            m.solve();

            if (m.solverStatus() == Cmpl.SOLVER_OK) {

                //standard report
                m.solutionReport();

                //example to obtain the solution manually 
                System.out.println("Objective name      :" + m.objectiveName());
                System.out.println("Nr. of Variables    :" + m.nrOfVariables());
                System.out.println("Nr. of Constraints  :" + m.nrOfConstraints());
                System.out.println("Nr. of Solutions    :" + m.nrOfSolutions());
                System.out.println("Display variables   :" + m.varDisplayOptions());
                System.out.println("Display constraints :" + m.varDisplayOptions());

                System.out.println("Solution nr.        :" + (m.solution().idx() + 1));
                System.out.println("Objective value     :" + m.solution().value() + " " + m.objectiveSense());

                for (CmplSolElement v : m.solution().variables()) {
                    System.out.println(v.idx() + " " + v.name() + " " + v.type() + " " + v.activity() + " " + v.lowerBound() + " " + v.upperBound() + " " + v.marginal());
                }
                for (CmplSolElement c : m.solution().constraints()) {
                    System.out.println(c.idx() + " " + c.name() + " " + c.type() + " " + c.activity() + " " + c.lowerBound() + " " + c.upperBound() + " " + c.marginal());
                }

                //example to obtain a variable by their name 
                CmplSolArray x = (CmplSolArray) m.getVarByName("x");
                for (int[] tuple : (int[][]) routes.values()) {
                    System.out.print(String.valueOf(tuple[0]) + " " + String.valueOf(tuple[1]) + " ");
                    System.out.println(x.get(tuple).activity());
                }

            } else {
                System.out.println("Solver failed " + m.solver() + " " + m.solverMessage());
            }

        } catch (CmplException e) {

            System.out.println(e);

        }
    }
}
