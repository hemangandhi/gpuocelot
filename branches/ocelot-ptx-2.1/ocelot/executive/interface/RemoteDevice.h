/*! \file RemoteDevice.h
	\author Andrew Kerr <arkerr@gatech.edu>, Gregory Diamos <gregory.diamos@gatech.edu>
	\date 26 Jan 2011
	\brief class defining a remote Ocelot device 
*/

#ifndef EXECUTIVE_REMOTEDEVICE_H_INCLUDED
#define EXECUTIVE_REMOTEDEVICE_H_INCLUDED

// C++ includes
#include <unordered_set>

// Ocelot includes
#include <ocelot/executive/interface/Device.h>

// Hydrazine includes
#include <hydrazine/implementation/Timer.h>

// Boost includes
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////

namespace executive
{
	class ExecutableKernel;
}

namespace executive 
{
	/*! class defining a remote Ocelot device */
	class RemoteDevice : public Device
	{
		public:
			/*! \brief An interface to a managed memory allocation */
			class MemoryAllocation : public Device::MemoryAllocation
			{
				private:
					/*! \brief The size of the allocation in bytes */
					size_t _size;
					/*! \brief This is in the host address space */
					void* _pointer;
					/*! \brief The flags for host allocated memory */
					unsigned int _flags;
					/*! \brief Is the allocation managed here or externally? */
					bool _external;
				
				public:
					/*! \brief Generic Construct */
					MemoryAllocation();
					/*! \brief Construct a device allocation */
					MemoryAllocation(size_t size);
					/*! \brief Construct a host allocation */
					MemoryAllocation(size_t size, unsigned int flags);
					/*! \brief Construct a global allocation */
					MemoryAllocation(const ir::Global& global);
					/*! \brief Construct an external allocaton */
					MemoryAllocation(void* pointer, size_t size);
					/*! \brief Desructor */
					~MemoryAllocation();

				public:
					/*! \brief Copy constructor */
					MemoryAllocation(const MemoryAllocation& a);
					/*! \brief Move constructor */
					MemoryAllocation(MemoryAllocation&& a);
					
					/*! \brief Assignment operator */
					MemoryAllocation& operator=(const MemoryAllocation& a);
					/*! \brief Move operator */
					MemoryAllocation& operator=(MemoryAllocation&& a);
			
				public:
					/*! \brief Get the flags if this is a host pointer */
					unsigned int flags() const;
					/*! \brief Get a host pointer if for a host allocation */
					void* mappedPointer() const;
					/*! \brief Get a device pointer to the base */
					void* pointer() const;
					/*! \brief The size of the allocation */
					size_t size() const;
					/*! \brief Copy from an external host pointer */
					void copy(size_t offset, const void* host, size_t size );
					/*! \brief Copy to an external host pointer */
					void copy(void* host, size_t offset, size_t size) const;
					/*! \brief Memset the allocation */
					void memset(size_t offset, int value, size_t size);
					/*! \brief Copy to another allocation */
					void copy(Device::MemoryAllocation* allocation, 
						size_t toOffset, size_t fromOffset, size_t size) const;				
			};

		protected:
			/*! \brief A class for holding the state associated with a module */
			class Module
			{
				public:
					/*! \brief This is a map from a global name to a pointer */
					typedef std::unordered_map<std::string, void*> GlobalMap;
					/*! \brief A map from a kernel name to its translation */
					typedef std::unordered_map<std::string, 
						ExecutableKernel*> KernelMap;
					/*! \brief A vector of memory allocations */
					typedef std::vector<MemoryAllocation*> AllocationVector;
					/*! \brief A map from texture names to references */
					typedef ir::Module::TextureMap TextureMap;
			
				public:
					/*! \brief The ir representation of a module */
					const ir::Module* ir;
					/*! \brief The emulator */
					Device* device;
					/*! \brief The set of global allocations in the module */
					GlobalMap globals;
					/*! \brief The set of translated kernels */
					KernelMap kernels;
					/*! \brief A duplicate copy of textures */
					TextureMap textures;
					
				public:
					/*! \brief Construct this based on a module */
					Module(const ir::Module* m = 0, Device* d = 0);
					/*! \brief Copy constructor */
					Module(const Module& m);
					/*! \brief Clean up all translated kernels */
					virtual ~Module();
					
				public:
					/*! \brief Load all of the globals for this module */
					AllocationVector loadGlobals();
					/*! \brief Get a specific kernel or 0 */
					virtual ExecutableKernel* getKernel(
						const std::string& name);
					/*! \brief Get a handle to a specific texture or 0 */
					ir::Texture* getTexture(const std::string& name);
			};

			/*! \brief A map of registered modules */
			typedef std::unordered_map<std::string, Module*> ModuleMap;

			/*! \brief A map of memory allocations */
			typedef std::map<void*, MemoryAllocation*> AllocationMap;
			
			/*! \brief A set of registered streams */
			typedef std::unordered_set<unsigned int> StreamSet;
			
			/*! \brief A map of registered events */
			typedef std::unordered_map<unsigned int, 
				hydrazine::Timer::Second> EventMap;

		protected:
			/*! \brief A map of memory allocations in device/host space */
			AllocationMap _allocations;
			
			/*! \brief The modules that have been loaded */
			ModuleMap _modules;
			
			/*! \brief Registered streams */
			StreamSet _streams;
			
			/*! \brief Registered events */
			EventMap _events;
		
			/*! \brief Has this device been selected? */
			bool _selected;
			
			/*! \brief The next handle to assign to an event, stream, etc */
			unsigned int _next;
			
			/*! \brief Global timer */
			hydrazine::Timer _timer;
			
		private:
		
			//! \brief IO service for RemoteDevice instance
			boost::asio::io_service _io_service;
			
			//! \brief socket for connecting to remote device
			boost::asio::ip::tcp::socket _socket;
						
		public:
			/*! \brief Sets the device properties, bind this to the cuda id */
			RemoteDevice(std::string host, int port, unsigned int flags = 0);
			/*! \brief Clears all state */
			virtual ~RemoteDevice();
			
		public:
			Device::MemoryAllocation* getMemoryAllocation(const void* address, 
				AllocationType type) const;
			/*! \brief Get the address of a global by stream */
			Device::MemoryAllocation* getGlobalAllocation(
				const std::string& module, const std::string& name);
			/*! \brief Allocate some new dynamic memory on this device */
			Device::MemoryAllocation* allocate(size_t size);
			/*! \brief Make this a host memory allocation */
			Device::MemoryAllocation* allocateHost(size_t size, 
				unsigned int flags);
			/*! \brief Free an existing non-global allocation */
			void free(void* pointer);
			/*! \brief Get nearby allocations to a pointer */
			MemoryAllocationVector getNearbyAllocations(void* pointer) const;
			/*! \brief Get all allocations, host, global, and device */
			MemoryAllocationVector getAllAllocations() const;
			/*! \brief Wipe all memory allocations, but keep modules */
			void clearMemory();			
			
		public:
			/*! \brief Load a module, must have a unique name */
			virtual void load(const ir::Module* module);
			/*! \brief Unload a module by name */
			void unload(const std::string& name);
			/*! \brief Get a translated kernel from the device */
			virtual ExecutableKernel* getKernel(const std::string& module, 
				const std::string& kernel);

		public:
			/*! \brief Create a new event */
			unsigned int createEvent(int flags);
			/*! \brief Destroy an existing event */
			void destroyEvent(unsigned int event);
			/*! \brief Query to see if an event has been recorded (yes/no) */
			bool queryEvent(unsigned int event) const;
			/*! \brief Record something happening on an event */
			void recordEvent(unsigned int event, unsigned int stream);
			/*! \brief Synchronize on an event */
			void synchronizeEvent(unsigned int event);
			/*! \brief Get the elapsed time in ms between two recorded events */
			float getEventTime(unsigned int start, unsigned int end) const;
		
		public:
			/*! \brief Create a new stream */
			unsigned int createStream();
			/*! \brief Destroy an existing stream */
			void destroyStream(unsigned int stream);
			/*! \brief Query the status of an existing stream (ready/not) */
			bool queryStream(unsigned int stream) const;
			/*! \brief Synchronize a particular stream */
			void synchronizeStream(unsigned int stream);
			/*! \brief Sets the current stream */
			void setStream(unsigned int stream);
			
		public:
			/*! \brief Select this device as the current device. 
				Only one device is allowed to be selected at any time. */
			void select();
			/*! \brief is this device selected? */
			bool selected() const;
			/*! \brief Deselect this device. */
			void unselect();
		
		public:
			/*! \brief Binds a texture to a memory allocation at a pointer */
			void bindTexture(void* pointer,
				const std::string& moduleName, const std::string& textureName, 
				const textureReference& ref, const cudaChannelFormatDesc& desc, 
				const ir::Dim3& size);
			/*! \brief unbinds anything bound to a particular texture */
			void unbindTexture(const std::string& moduleName, 
				const std::string& textureName);
			/*! \brief Get's a reference to an internal texture */
			void* getTextureReference(const std::string& moduleName, 
				const std::string& textureName);

		public:
			/*! \brief helper function for launching a kernel
				\param module module name
				\param kernel kernel name
				\param grid grid dimensions
				\param block block dimensions
				\param sharedMemory shared memory size
				\param argumentBlock array of bytes for parameter memory
				\param argumentBlockSize number of bytes in parameter memory
				\param traceGenerators vector of trace generators to add 
					and remove from kernel
			*/
			virtual void launch(const std::string& module, 
				const std::string& kernel, const ir::Dim3& grid, 
				const ir::Dim3& block, size_t sharedMemory, 
				const void* argumentBlock, size_t argumentBlockSize, 
				const trace::TraceGeneratorVector& 
					traceGenerators = trace::TraceGeneratorVector());
					
			/*! \brief Get the function attributes of a specific kernel */
			cudaFuncAttributes getAttributes(const std::string& module, 
				const std::string& kernel);
			/*! \brief Get the last error from this device */
			unsigned int getLastError() const;
			/*! \brief Wait until all asynchronous operations have completed */
			void synchronize();
			
		public:
			/*! \brief Limit the worker threads used by this device */
			virtual void limitWorkerThreads(unsigned int threads);			
			/*! \brief Set the optimization level for kernels in this device */
			virtual void setOptimizationLevel(
				translator::Translator::OptimizationLevel level);
	};
}

/////////////////////////////////////////////////////////////////////////////////////////////////

#endif