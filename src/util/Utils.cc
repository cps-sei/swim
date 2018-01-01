/*******************************************************************************
 * Simulator of Web Infrastructure and Management
 * Copyright (c) 2016 Carnegie Mellon University.
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
 * MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
 * ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
 *
 * Released under a BSD license, please see license.txt for full terms.
 * DM-0003883
 *******************************************************************************/

#include "Utils.h"
#include <string.h>
#include <boost/tokenizer.hpp>

using namespace std;

double Utils::getMeanAndVarianceFromParameter(const omnetpp::cPar& par, double* variance) {
    if (par.isExpression()) {
        string expr = par.getExpression()->str();

        // assume it's a distribution
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        tokenizer tokens(expr, boost::char_separator<char>("(,)"));
        tokenizer::iterator it = tokens.begin();
        ASSERT(it != tokens.end());
        if (*it == "exponential") {
            ASSERT(++it != tokens.end());
            double mean = atof((*it).c_str());
            if (variance) {
                *variance = pow(mean, 2);
            }
            return mean;
        } else if (*it == "normal" || *it == "truncnormal") {
            ASSERT(++it != tokens.end());
            double mean = atof((*it).c_str());
            ASSERT(++it != tokens.end());
            if (variance) {
                *variance = pow(atof((*it).c_str()), 2);
            }
            return mean;
        }
        cerr << "random distribution " << expr << " not supported. Aborting..." << endl;
        ASSERT(false); // distribution not supported;
    } else {
        // is a constant
        if (variance) {
            *variance = 0;
        }

        /*
         * try to see if there is a -const param associated with this
         * This is used to get the constant equivalent of a param that was defined with
         * an iteration variable, and therefore, isExpression is false, even though
         * it has an expression
         */
        string constParName(par.getName());
        constParName += "_const";
        auto parOwner = (omnetpp::cComponent*) par.getOwner();
        if (parOwner->hasPar(constParName.c_str())) {
            return parOwner->par(constParName.c_str()).doubleValue();
        }

        return par.doubleValue();
    }
    return 0;
}


Utils::~Utils() {
    // TODO Auto-generated destructor stub
}

