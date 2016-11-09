/* ****************************************************************************
 * This code is part of jCMPL
 *
 * Copyright (C) 2013 Mike Steglich / B. Knie Technical University of Applied
 * Sciences Wildau, Germany
 *
 * jCMPL is a project of the Technical University of Applied Sciences Wildau
 * and the Institute for Operations Research and Business Management at the
 * Martin Luther University Halle-Wittenberg.
 *
 * Please visit the project homepage <http://www.coliop.org>
 *
 * jCMPL is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * jCMPL is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public # License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

package jcmpltest;

import jCMPL.*;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

import java.util.ArrayList;

/**
 *
 * @author mike
 */
public class Diet {

    public Diet(String fName, int mode) throws CmplException {
        try {

            Cmpl model = new Cmpl("diet.cmpl");

            CmplSet nutr = new CmplSet("NUTR");
            ArrayList nutrLst = new ArrayList<String>();
            nutrLst.add("A");
            nutrLst.add("B1");
            nutrLst.add("B2");
            nutrLst.add("C");
            nutr.setValues(nutrLst);

            CmplSet food = new CmplSet("FOOD");
            String[] foodLst = {"BEEF", "CHK", "FISH", "HAM", "MCH", "MTL", "SPG", "TUR"};
            food.setValues(foodLst);


            CmplParameter costs = new CmplParameter("costs", food);
            Double[] costVec = {3.19, 2.59, 2.29, 2.89, 1.89, 1.99, 1.99, 2.49};
            costs.setValues(costVec);

            CmplParameter vitmin = new CmplParameter("vitMin", nutr);
            ArrayList vitminVec = new ArrayList();
            vitminVec.add(700);
            vitminVec.add(700);
            vitminVec.add(700);
            vitminVec.add(700);
            vitmin.setValues(vitminVec);

            CmplParameter vitamin = new CmplParameter("vitamin", nutr, food);
            int[][] vitMat = {{60, 8, 8, 40, 15, 70, 25, 60}, {20, 0, 10, 40, 35, 30, 50, 20}, {10, 20, 15, 35, 15, 15, 25, 15}, {15, 20, 10, 10, 15, 15, 15, 10}};
            vitamin.setValues(vitMat);


            model.setSets(nutr, food);
            model.setParameters(costs, vitmin, vitamin);

            model.setOption("%arg -solver glpk");

            if (mode == 1) {
                model.connect("http://127.0.0.1:9999");
            }

            //model.debug(true);
            model.solve();
            BufferedWriter out = new BufferedWriter(new FileWriter(fName + ".stdout"));

            if (model.solverStatus() == Cmpl.SOLVER_OK) {

                out.write("Objective name      :" + model.objectiveName() + "\n");
                out.write("Nr. of Variables    :" + model.nrOfVariables() + "\n");
                out.write("Nr. of Constraints  :" + model.nrOfConstraints() + "\n");
                out.write("Nr. of Solutions    :" + model.nrOfSolutions() + "\n");
                out.write("Display variables   :" + model.varDisplayOptions() + "\n");
                out.write("Display constraints :" + model.varDisplayOptions() + "\n");

                for (CmplSolution s : model.solutionPool()) {

                    out.write("Solution nr.        :" + (s.idx() + 1) + "\n");
                    out.write("Objective value     :" + s.value() + " " + model.objectiveSense() + "\n");

                    for (CmplSolElement v : s.variables()) {
                        out.write(v.idx() + " " + v.name() + " " + v.type() + " " + v.activity() + " " + v.lowerBound() + " " + v.upperBound() + " " + v.marginal() + "\n");
                    }
                    for (CmplSolElement c : s.constraints()) {
                        out.write(c.idx() + " " + c.name() + " " + c.type() + " " + c.activity() + " " + c.lowerBound() + " " + c.upperBound() + " " + c.marginal() + "\n");
                    }
                }


                out.write("Solution nr.        :" + (model.solution().idx() + 1) + "\n");
                out.write("Objective value     :" + model.solution().value() + " " + model.objectiveSense() + "\n");

                for (CmplSolElement v : model.solution().variables()) {
                    out.write(v.idx() + " " + v.name() + " " + v.type() + " " + v.activity() + " " + v.lowerBound() + " " + v.upperBound() + " " + v.marginal() + "\n");
                }
                for (CmplSolElement c : model.solution().constraints()) {
                    out.write(c.idx() + " " + c.name() + " " + c.type() + " " + c.activity() + " " + c.lowerBound() + " " + c.upperBound() + " " + c.marginal() + "\n");
                }

                model.saveSolution(fName + ".csol");
                model.saveSolutionAscii(fName + ".sol");
                model.saveSolutionCsv(fName + ".csv");

                CmplSolArray x = (CmplSolArray) model.getVarByName("x");
                out.write(x.get("BEEF").activity() + "\n");
            } else {
                out.write("Solver failed " + model.solver() + " " + model.solverMessage());
            }

            out.close();

        } catch (IOException e) {
            System.out.println("IO error for test diet  " + e);
        } catch (CmplException e) {
            System.out.println(e);
        }
    }
}
