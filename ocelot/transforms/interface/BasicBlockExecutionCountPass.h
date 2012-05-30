/*! \file BasicBlockExecutionCountPass.h
	\author Naila Farooqui <naila@cc.gatech.edu>
	\date Wednesday October 6, 2010
	\brief The header file for the BasicBlockExecutionCountPass class.
*/

#ifndef BASIC_BLOCK_EXECUTION_COUNT_PASS_H_INCLUDED
#define BASIC_BLOCK_EXECUTION_COUNT_PASS_H_INCLUDED

#include <ocelot/analysis/interface/DataflowGraph.h>
#include <ocelot/ir/interface/PTXKernel.h>
#include <ocelot/transforms/interface/BasicBlockInstrumentationPass.h>

namespace ir
{
	class Module;
}

namespace transforms
{
	/*! \brief Implements the basic block execution count instrumentation */
	class BasicBlockExecutionCountPass : public BasicBlockInstrumentationPass
	{
	    public:
            /*! \brief default constructor */
            BasicBlockExecutionCountPass();

        protected:

            size_t incrementBasicBlockCounter(analysis::DataflowGraph::iterator block, analysis::DataflowGraph::RegisterId registerId, std::map<std::string, analysis::DataflowGraph::RegisterId> registerMap, size_t location);

        public:
			/*! \brief Run the pass on a specific kernel */
			void runOnKernel( ir::IRKernel& k );
	};
}

#endif

