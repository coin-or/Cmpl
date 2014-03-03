
/**
 * *************************************************************************
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
 * *************************************************************************
 */
import jCMPL.*;
import java.io.PrintStream;
import java.util.ArrayList;

public class CuttingStock {

    public static void main(String[] args) throws CmplException {

        try {
            Cmpl cuttingOpt = new Cmpl("cut.cmpl");
            Cmpl patternGen = new Cmpl("cut-pattern.cmpl");

            CmplParameter r = new CmplParameter("rollWidth");
            r.setValues(110);

            CmplSet w = new CmplSet("widths");
            int[] wVals = {20, 45, 50, 55, 75};
            w.setValues(wVals);

            CmplParameter o = new CmplParameter("orders", w);
            int[] oVals = {48, 35, 24, 10, 8};
            o.setValues(oVals);

            int nPat = w.len();

            CmplSet p = new CmplSet("patterns");
            p.setValues(1, nPat);

            ArrayList<ArrayList<Long>> nbr = new ArrayList<>();

            for (int i = 0; i < nPat; i++) {
                ArrayList<Long> nbrRow = new ArrayList<>();
                for (int j = 0; j < nPat; j++) {
                    if (i == j) {
                        Double nr = Math.floor(((Integer) r.value()) / ((int[]) w.values())[i]);
                        nbrRow.add(nr.longValue());
                    } else {
                        nbrRow.add(Long.valueOf(0));
                    }
                }
                nbr.add(nbrRow);
            }

            CmplParameter n = new CmplParameter("nbr", w, p);
            n.setValues(nbr);

            Double[] price = new Double[w.len()];
            for (int i = 0; i < price.length; i++) {
                price[i] = 0.0;
            }

            CmplParameter pr = new CmplParameter("price", w);
            pr.setValues(price);

            cuttingOpt.setSets(w, p);
            cuttingOpt.setParameters(r, o, n);

            patternGen.setSets(w);
            patternGen.setParameters(r, pr);

            int ri = cuttingOpt.setOption("%arg -integerRelaxation");

            while (true) {
                cuttingOpt.solve();

                CmplSolArray fill = (CmplSolArray) cuttingOpt.getConByName("fill");

                int pos = 0;
                for (int with : (int[]) w.values()) {
                    price[pos] = fill.get(with).marginal();
                    pos++;
                }

                pr.setValues(price);

                patternGen.solve();
                CmplSolArray use = (CmplSolArray) patternGen.getVarByName("use");

                if (1 - patternGen.solution().value() < -0.00001) {
                    nPat++;
                    p.setValues(1, nPat);
                    for (int i = 0; i < w.len(); i++) {
                        ArrayList<Long> tmpList = nbr.get(i);
                        tmpList.add((Long) use.get(w.get(i)).activity());
                        nbr.set(i, tmpList);

                    }
                    n.setValues(nbr);
                } else {
                    break;
                }
            }
            cuttingOpt.delOption(ri);

            cuttingOpt.solve();
            CmplSolArray cut = (CmplSolArray) cuttingOpt.getVarByName("cut");

            final PrintStream out = System.out;

            out.printf("Objective value: %4.2f%n%n", cuttingOpt.solution().value());

            out.printf("Pattern:\n");
            out.printf("   | ");
            for (int j : (ArrayList<Integer>) p.values()) {
                out.printf(" %d ", j);
            }
            out.printf("\n----------------------------\n");
            for (int i = 0; i < w.len(); i++) {
                out.printf("%2d | ", w.get(i));
                for (int j : (ArrayList<Integer>) p.values()) {
                    out.printf(" %d ", nbr.get(i).get(j - 1));
                }
                out.printf("\n");
            }
            out.printf("\n");
            for (int j : (ArrayList<Integer>) p.values()) {
                if ((Long) cut.get(j).activity() > 0) {
                    out.printf("%2d pieces of pattern: %d %n", (Long) cut.get(j).activity(), j);
                    for (int i = 0; i < w.len(); i++) {
                        out.printf("\twidth %d - %d%n", w.get(i), nbr.get(i).get(j - 1));
                    }
                }
            }
            out.close();
        } catch (CmplException e) {
            System.err.println(e);
        }
    }
}
