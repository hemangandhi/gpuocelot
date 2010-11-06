/*! \file CountBasicBlockExecutionPass.h
	\author Naila Farooqui <naila@cc.gatech.edu>
	\date Wednesday October 6, 2010
	\brief The header file for the CountBasicBlockExecutionPass class.
*/

#ifndef COUNT_BASIC_BLOCK_EXECUTION_PASS_H_INCLUDED
#define COUNT_BASIC_BLOCK_EXECUTION_PASS_H_INCLUDED

#include <ocelot/analysis/interface/DataflowGraph.h>
#include <ocelot/ir/interface/PTXKernel.h>
#include <ocelot/analysis/interface/Pass.h>

namespace ir
{
	class Module;
}

namespace analysis
{
	/*! \brief Implements the basic block execution counter */
	class CountBasicBlockExecutionPass : public ModulePass
	{
	    public:
			/*! \brief The id of the basic block counter base pointer */			
             std::string basicBlockCounterBase() const;
			
		public:
			CountBasicBlockExecutionPass();	
			/*! \brief Initialize the pass using a specific module */
			void initialize( const ir::Module& m );
			/*! \brief Run the pass on a specific module */
			void runOnModule( ir::Module& m );
			/*! \brief Finalize the pass */
			void finalize( );

        private:
            DataflowGraph::RegisterId _runOnEntryBlock( ir::PTXKernel* kernel, DataflowGraph::iterator block);
			void _runOnBlock( ir::PTXKernel* kernel, DataflowGraph::iterator block, DataflowGraph::RegisterId counterPtrRegId, DataflowGraph::RegisterId registerId, unsigned int offset );
			
	};
}

#endif

