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

public class ShortestPath {

    public ShortestPath() throws CmplException {

        try {
            Cmpl m = new Cmpl("shortest-path.cmpl");

            CmplSet routes = new CmplSet("A", 2);
            int[][] arcs = {{1, 2}, {1, 4}, {2, 1}, {2, 3}, {2, 4}, {2, 5},
                {3, 2}, {3, 5}, {4, 1}, {4, 2}, {4, 5}, {4, 6},
                {5, 2}, {5, 3}, {5, 4}, {5, 6}, {5, 7},
                {6, 4}, {6, 5}, {6, 7}, {7, 5}, {7, 6}};
            routes.setValues(arcs);

            CmplSet nodes = new CmplSet("V");
            nodes.setValues(1, 7);

            CmplParameter c = new CmplParameter("c", routes);
            Integer[] cArr = {7, 5, 7, 8, 9, 7, 8, 5, 5, 9, 15, 6, 7, 5, 15, 8, 9, 6, 8, 11, 9, 11};
            c.setValues(cArr);

            CmplParameter sNode = new CmplParameter("s");
            sNode.setValues(1);

            CmplParameter tNode = new CmplParameter("t");
            tNode.setValues(7);

            m.setSets(routes, nodes);
            m.setParameters(c, sNode, tNode);

            m.setOption("%display nonZeros");

            m.solve();

            BufferedWriter out = new BufferedWriter(new FileWriter("shortest-path-local" + ".stdout"));

            if (m.solverStatus() == Cmpl.SOLVER_OK) {

                out.write("Objective value     :" + m.solution().value() + "\n");

                for (CmplSolElement v : m.solution().variables()) {
                    out.write(v.name() + " " + v.activity() + "\n");
                }
                m.saveSolution("shortest-path-local.csol");
                m.saveSolutionAscii("shortest-path-local.sol");
                m.saveSolutionCsv("shortest-path-local.csv");

            } else {
                out.write("Solver failed " + m.solver() + " " + m.solverMessage());
            }

            out.close();

        } catch (IOException ex) {
            System.out.println("IO error for test shortest-path  " + ex);
        } catch (CmplException e) {
            System.out.println(e);
        }
    }
}
