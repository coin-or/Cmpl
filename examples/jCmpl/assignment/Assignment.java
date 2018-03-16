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

public class Assignment {

    public static void main(String[] args) throws CmplException {

        try {

            Cmpl m = new Cmpl("assignment.cmpl");

            CmplSet locations = new CmplSet("locations");
            locations.setValues(1, 4);

            CmplSet machines = new CmplSet("machines");
            machines.setValues(1, 3);

            CmplSet combinations = new CmplSet("A", 2);
            int[][] combiVals = {{1, 1}, {1, 2}, {1, 3}, {1, 4}, {2, 1}, {2, 3}, {2, 4}, {3, 1}, {3, 2}, {3, 3}, {3, 4}};
            combinations.setValues(combiVals);

            CmplParameter costs = new CmplParameter("c", combinations);
            int[] costVals = {13, 16, 12, 11, 15, 13, 20, 5, 7, 10, 6};
            costs.setValues(costVals);

            m.setSets(machines, locations, combinations);
            m.setParameters(costs);

            m.setOption("%display nonZeros");
            m.setOption("%arg -solver cplex");
            m.setOption("%display solutionPool");

            //m.debug(true);
            m.solve();
            

            if (m.solverStatus() == Cmpl.SOLVER_OK) {

                for (CmplSolution s : m.solutionPool()) {

                    System.out.printf("Solution number:  %d %n", (s.idx() + 1));
                    System.out.printf("Objective value:  %f %n", s.value());
                    System.out.printf("Objective status: %s %n", s.status());

                    System.out.println("Variables:");
                    for (CmplSolElement v : s.variables()) {
                        System.out.printf("%10s %3s %10d %10.0f %10.0f%n", v.name(), v.type(), v.activity(), v.lowerBound(), v.upperBound());
                    }
                    System.out.println("Constraints:");
                    for (CmplSolElement c : s.constraints()) {
                        System.out.printf("%10s %3s %10.0f %10.0f %10.0f%n", c.name(), c.type(), c.activity(), c.lowerBound(), c.upperBound());
                    }
                }
                
            } else {
                System.out.println("Solver failed " + m.solver() + " " + m.solverMessage());
            }

        } catch (CmplException e) {
            System.out.println(e);
        }
    }
}
