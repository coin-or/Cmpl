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

import jCMPL.CmplException;

/**
 *
 * @author mike
 */
public class JCmplTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws CmplException {

        System.out.println("Creating test files ");

        Diet diet1 = new Diet("diet-local", 0);
        System.out.println("... diet-local");
        ShortestPath sp = new ShortestPath();
        System.out.println("... shortest-path");
        CuttingStock cut = new CuttingStock();
        System.out.println("... cuttingStock");
        ProdMix p = new ProdMix();
        System.out.println("... prodMix");
        


        if (args[0].equals("1")) {
            Diet diet2 = new Diet("diet-remote", 1);
            System.out.println("... diet-remote");
            shortestPathThreads st = new shortestPathThreads();
            System.out.println("... shortestPathThreads");
        }

        System.out.println("done");


    }
}
