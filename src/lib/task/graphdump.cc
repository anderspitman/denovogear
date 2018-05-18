/*
 * Copyright (c) 2016-2018 Reed A. Cartwright
 * Authors:  Reed A. Cartwright <reed@cartwrig.ht>
 *
 * This file is part of DeNovoGear.
 *
 * DeNovoGear is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <dng/task/graphdump.h>
#include <dng/io/fasta.h>
#include <dng/io/bam.h>
#include <dng/io/ad.h>
#include <dng/io/bcf.h>
#include <dng/relationship_graph.h>

using namespace dng;
using namespace dng::task;

// Sub-tasks
namespace {
int process_bam(GraphDump::argument_type &arg);
int process_ad(GraphDump::argument_type &arg);
int process_bcf(GraphDump::argument_type &arg);
} // anon namespace

// The main loop for dng-graphdump application
// argument_type arg holds the processed command line arguments
int task::GraphDump::operator()(task::GraphDump::argument_type &arg) {
    using utility::FileCat;
    using utility::FileCatSet;
    // if input is empty default to stdin.
    if(arg.input.empty()) {
        arg.input.emplace_back("-");
    }

    // Check that all input formats are of same category
    auto it = arg.input.begin();
    FileCat mode = utility::input_category(*it, FileCat::Sequence|FileCat::Pileup|FileCat::Variant, FileCat::Sequence);
    for(++it; it != arg.input.end(); ++it) {
        if(utility::input_category(*it, FileCat::Sequence|FileCat::Pileup|FileCat::Variant, FileCat::Sequence) != mode) {
            throw std::invalid_argument("Mixing sam/bam/cram, vcf/bcf, and tad/ad input files is not supported.");
        }
    }
    // Execute sub tasks based on input type
    if(mode == FileCat::Pileup) {
        return process_ad(arg);
    } else if(mode == FileCat::Variant) {
        // vcf, bcf
        return process_bcf(arg);
    } else if(mode == FileCat::Sequence) {
        return process_bam(arg);
    } else {
        throw std::invalid_argument("Unknown input data file type.");
    }
    return EXIT_FAILURE;
}

namespace {
int process_bam(GraphDump::argument_type &arg) {
    // Open Reference
    if(arg.fasta.empty()){
        throw std::invalid_argument("Path to reference file must be specified with --fasta when processing bam/sam/cram files.");
    }
    io::Fasta reference{arg.fasta.c_str()};

    // Open input files
    auto mpileup = io::BamPileup::open_and_setup(arg);

    auto relationship_graph = create_relationship_graph(arg, &mpileup);

    return EXIT_SUCCESS;
}

int process_ad(GraphDump::argument_type &arg) {
    using namespace std;
   
    // Read input data
    auto mpileup = io::AdPileup::open_and_setup(arg);

    auto relationship_graph = create_relationship_graph(arg, &mpileup);

    return EXIT_SUCCESS;
}

int process_bcf(GraphDump::argument_type &arg) {
    // Read input data
    auto mpileup = io::BcfPileup::open_and_setup(arg);

    auto relationship_graph = create_relationship_graph(arg, &mpileup);


    return EXIT_SUCCESS;
}
}
