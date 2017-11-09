
/*
 * =====================================================================================
 * 
 * Copyright (c) 2012 Linköping University
 * Author: Lu Li <lu.li@liu.se>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */




#ifndef METERPU_INCLUSION_GUARD
#define METERPU_INCLUSION_GUARD

//Headers
/*{{{*/

/*
 * =====================================================================================
 *
 *       Filename:  MeterPU.h
 *
 *    Description:  MeterPU: Library for measurement Abstraction
 *        Version:  0.8
 *
 *         Author:  Lu Li (lu.li@liu.se, explore.leo@gmail.com)
 *   Organization:  IDA (www.ida.liu.se)
 *
 * =====================================================================================
 */


//Module dependence
/*{{{*/
#ifdef ENABLE_SYSTEM_ENERGY //SYSTEM_ENERGY depends on PCM and NVML

#ifndef ENABLE_PCM
#define ENABLE_PCM
#endif

#ifndef ENABLE_NVML
#define ENABLE_NVML
#endif

#endif

#ifdef ENABLE_NVML
#include <nvml.h>
#endif 

#ifdef ENABLE_PCM
#include <cpucounters.h>    //for PCM_Energy 
#endif
/*}}}*/

//Header files
/*{{{*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <unistd.h>                     
#include <assert.h>
/*}}}*/

//Error message handling
/*{{{*/
#define PRINT_FUNC_NAME(message) std::cout<<"["<<__FILE__<<"#"<<__LINE__<<" "<<__func__<<"():] "<<message<<std::endl
#define PRINT_FUNC_NAME_CONT std::cout<<"["<<__FILE__<<"#"<<__LINE__<<" "<<__func__<<"():] "

#define DECLARE_CLASS_NAME(name) static std::string class_name(){return name;}
#define PRINT_CLASS_FUNC_NAME(message) std::cout<<"["<<__FILE__<<"#"<<__LINE__<<" "<<class_name()<<" "<<__func__<<"():] "<<message<<std::endl
#define PRINT_CLASS_FUNC_NAME_CONT std::cout<<"["<<__FILE__<<"#"<<__LINE__<<" "<<class_name()<<" "<<__func__<<"():] "
/*}}}*/

/*}}}*/ 

//Doxygen Main page
/*{{{*/
/*! \mainpage MeterPU
 *
 * \section intro_sec Introduction
 *
 * MeterPU: Library for measuring different metrics on different backends.
 *
 * \section install_sec Installation
 *
 * - Include "MeterPU.h" in your program
 * - Compile your program with native measurement library paths if necessary.
 * 	- E.g. if CUDA Energy Meter is used, compile your program with NVML library by passing -I and -L flag.
 * 	- For CPU Time Meter, no library is needed.
 * 	- Also pass the following flag if the following Meter is used.
 * 		- CUDA Time Meter: -DENABLE_CUDA_TIME
 * 		- CPU and DRAM Energy Meter: -DENABLE_PCM
 * 		- CUDA GPU Energy Meter: -DENABLE_NVML
 * 		- System Energy Meter: -DENABLE_SYSTEM_ENERGY
 * - Done!
 *
 * See more examples in example folder.
 */
/*}}}*/

//Globals
/*{{{*/
//Macros
/*{{{*/
#define METERPU_TIME_MEASURE(x) clock_gettime(CLOCK_MONOTONIC, x)
/*}}}*/

//Functions
/*{{{*/

inline bool bash_exe (const std::string& cmd,std::vector<std::string>& out ) {
    FILE*           fp;
    const int       SIZEBUF = 1234;
    char            buf [SIZEBUF];
    out = std::vector<std::string> ();
    if ((fp = popen(cmd.c_str (), "r")) == NULL) {
        return false;
    }
    std::string  cur_string = "";
    while (fgets(buf, sizeof (buf), fp)) {
        cur_string += buf;
    }
    out.push_back (cur_string.substr (0, cur_string.size () - 1));
    pclose(fp);
    return true;
}

/*}}}*/

/*}}}*/

//Implementation
/*{{{*/
namespace MeterPU
{

	//forward declaration
	/*{{{*/

	

	template<class Type> 
		struct Meter_Traits;
	struct Measurement_Controller;

	struct CPU_Time_Environment_Init;
	struct CPU_Time;
	struct CPU_Time_Measurement_Controller;

#ifdef ENABLE_CUDA_TIME
	struct CUDA_Time_Environment_Init;
	struct CUDA_Time;
	struct CUDA_Time_Measurement_Controller;
#endif

#ifdef ENABLE_NVML
	typedef unsigned int GPU_Device_Id_Type;
	struct NVML_Energy_Environment_Init;
	template<GPU_Device_Id_Type device_id=0>
		struct NVML_Energy;
	template<GPU_Device_Id_Type device_id>
		struct NVML_Energy_Measurement_Controller;
#endif

#ifdef ENABLE_PCM
	struct PCM_Energy_Environment_Init;
	struct PCM_Energy;
	struct PCM_Energy_Measurement_Controller;
#endif

#ifdef ENABLE_SYSTEM_ENERGY
	struct System_Energy_Environment_Init;
	template <GPU_Device_Id_Type... gpu_ids>
		struct System_Energy;
	template<GPU_Device_Id_Type ...gpu_device_ids>
	struct System_Energy_Measurement_Controller;
#endif


	/*}}}*/

	//group0: Interface
	/*{{{*/
	/** @defgroup group0 Interface
	 *  Describe interface exposed to the user programs.
	 *  @{
	 */



	/**
	 * \brief The software multi-meters.
	 *
	 * The only class the user program 
	 * should use. When equipped with different template parameter,
	 * it can measure different metrics on code regions.
	 * So far supported metrics are 
	 */
	template<class Type> class Meter
	{
		public:
			Meter()
			{
				environment_init_object.init();
				measurement_controller_object.init();
			}
			/**
			 * \brief start a measurement
			 */
			inline void start(){measurement_controller_object.start();}
			/**
			 * \brief stop a measurement
			 */
			inline void stop(){measurement_controller_object.stop();}
			/**
			 * \brief calculate measurement value
			 */
			inline void calc()
			{
				measurement_controller_object.calc();
				meter_reading = measurement_controller_object.get_value();
			}
			/**
			 *  \brief Get calculated metric value, require calc() to be
			 *  called already.
			 *  
			 *  
			 */
			inline typename Meter_Traits<Type>::ResultType const &get_value() const {return meter_reading;}
			/**
			 *  \brief Write measurement value on standard output
			 *  with its unit.
			 *  
			 *  
			 */
			inline void show_meter_reading(){measurement_controller_object.show_meter_reading();}
			//Remove it from release version
#ifdef METERPU_TEST
			void test(){measurement_controller_object.test();}
#endif
		private:
			//! Native library initializer.
			/*!
			  For GPU, initialize NVML library.
			  */
			typename Meter_Traits<Type>::Environment_Init_Type environment_init_object;
			//! Measurement controller.
			/*!
			 * The object that do the measurement, Meter class pass
			 * control signal such as start and stop to it.
			 */
			typename Meter_Traits<Type>::Measurement_Controller measurement_controller_object;
			//! The variable used to store the calculated
			//measurement value.
			/*!
			  
			*/
			typename Meter_Traits<Type>::ResultType meter_reading;
	};



	/** @} */ // end of group0
	/*}}}*/

	//group1: Traits
	/*{{{*/

	//group1 Traits for Different Meter Type
	/*{{{*/
	/** @defgroup group1 Traits for Different Meter Type 
	 *  Distinguish between Energy and Time etc.
	 *  @{
	 */

	//!  Traits Interface. 
	/*!
	  If initialized with a type for measurement, 
	  all types stores in Meter_Traits changes accordingly.
	  */
	template<class Type> struct Meter_Traits
	{
		//! Object for initialization of a native measurement library.
		/*!
		  <a href="index.html">MeterPU</a> use native measurement libraries for measurement, e.g. NVML for GPU. If a native library needs initialization, then the initialization class should be implemented.
		*/
		typedef typename Type::Environment_Init_Type Environment_Init_Type;
		//! Result type.
		/*!
		  
		*/
		typedef typename Type::ResultType ResultType;
		//! Measurement Controller Object.
		/*!
		  The Measurement Controller class implement functionality needed to start, stop and calculation of a measurement by a native measurement library.
		*/
		typedef typename Type::Measurement_Controller Measurement_Controller;

	};

	//!  Time Traits. 
	/*!
	  Describe types relevant to CPU time measurement. 
	  */
	struct CPU_Time
	{
		typedef CPU_Time_Environment_Init Environment_Init_Type;
		typedef double ResultType;
		typedef CPU_Time_Measurement_Controller Measurement_Controller;
		//! Time stamp unit.
		/*!
		   Different with ResultType,
		   since the latter is the elapsed time.
		*/
		typedef struct timespec Time_Unit;
	};



	/**
	 *  \brief Calculate elapsed time between two time stamp.
	 *  
	 *  
	 */
	inline CPU_Time::ResultType operator-(CPU_Time::Time_Unit const &stop_time,CPU_Time::Time_Unit const &start_time)
	{
		CPU_Time::ResultType elapse_time = (static_cast<double>(stop_time.tv_sec - start_time.tv_sec)*1000000.0+static_cast<double>(stop_time.tv_nsec - start_time.tv_nsec)/1000.0);
#ifdef DEBUG_OPERATOR_MINUS
		if(elapse_time<0){
			std::cout<<"stop_time.tv_sec: "<<stop_time.tv_sec<<"start_time.tv_sec: "<<start_time.tv_sec<<std::endl;
			std::cout<<"stop_time.tv_nsec: "<<stop_time.tv_nsec<<"start_time.tv_nsec: "<<start_time.tv_nsec<<std::endl;
		}
#endif
		assert(elapse_time>=0);
		return elapse_time;
	}
	/**
	 *  \brief Check if a time stamp is earlier than another.
	 *  
	 *  
	 */
	inline bool operator<(CPU_Time::Time_Unit const &small_time,CPU_Time::Time_Unit const &large_time)
	{
		if(small_time.tv_sec<large_time.tv_sec)
			return true;
		if(small_time.tv_sec>large_time.tv_sec)
			return false;
		if(small_time.tv_nsec<large_time.tv_nsec)
			return true;
		return false;
	}
	/**
	 *  \brief Check if two time stamps are the same.
	 *  
	 *  
	 */
	inline bool operator==(CPU_Time::Time_Unit const &small_time,CPU_Time::Time_Unit const &large_time)
	{
		if(small_time.tv_sec!=large_time.tv_sec)
			return false;
		if(small_time.tv_nsec!=large_time.tv_nsec)
			return false;
		return true;
	}
	/**
	 *  \brief Check if a time stamp is earlier or equal to another.
	 *  
	 *  
	 */
	inline bool operator<=(CPU_Time::Time_Unit const &small_time,CPU_Time::Time_Unit const &large_time)
	{
			return (small_time<large_time || small_time == large_time);
	}
	//using namespace std::rel_ops;
	

#ifdef ENABLE_CUDA_TIME
	//!  Cuda Timer Traits. 
	/*!
	  Describe types relevant to GPU time measurement. 
	*/
	struct CUDA_Time
	{
		typedef CUDA_Time_Environment_Init Environment_Init_Type;
		typedef float ResultType;
		typedef CUDA_Time_Measurement_Controller Measurement_Controller;
	};
#endif

#ifdef ENABLE_PCM
	//!  PCM Energy Traits. 
	/*!
	  Describe types relevant to CPU and DRAM Energy measurement.
	*/
	struct PCM_Energy
	{
		typedef PCM_Energy_Environment_Init Environment_Init_Type;
		typedef double ResultType;
		typedef PCM_Energy_Measurement_Controller Measurement_Controller;
		//lu/ Type-specific
		typedef double CPU_Energy_Type;
		typedef double DRAM_Energy_Type;
	};
#endif

#ifdef ENABLE_NVML


	
	//! GPU Energy Traits. 
	/*!
	  Describe types relevant to energy measurement.
	  Customed GPU device id is possible by template argument.
	  */
	template<GPU_Device_Id_Type device_id>
		struct NVML_Energy
		{
			typedef double Energy_Unit;

			typedef NVML_Energy_Environment_Init Environment_Init_Type;
			typedef Energy_Unit ResultType;
			typedef NVML_Energy_Measurement_Controller<device_id> Measurement_Controller;

		
			typedef struct timespec Time_Unit;
			typedef unsigned int Power_Unit;
			//! High precision power unit.
			/*!
			  Used for calculation with minimal precision loss.
			  */
			typedef long double Hp_Power_Unit;
			typedef std::vector<Time_Unit> Time_DB_Type;
			typedef Time_DB_Type::const_iterator Time_DB_Const_Iterator_Type;
			typedef std::vector<Power_Unit> Power_DB_Type;
			typedef std::vector<Hp_Power_Unit> Hp_Power_DB_Type;
			typedef Power_DB_Type::const_iterator Power_DB_Const_Iterator_Type;
			typedef Hp_Power_DB_Type::const_iterator Hp_Power_DB_Const_Iterator_Type;

		};
#endif

#ifdef ENABLE_SYSTEM_ENERGY
	//!  System Energy Traits. 
	/*!
	  Describe types relevant to energy measurement.
	  System Energy Meter can be specified as a combination of all CPUs and some GPUs.
	*/
	template <GPU_Device_Id_Type... gpu_ids>
		struct System_Energy
		{
			typedef System_Energy_Environment_Init Environment_Init_Type;
			typedef System_Energy_Measurement_Controller<gpu_ids...> Measurement_Controller;
			typedef double ResultType;
		};
#endif


	/** @} */ // end of group1
	/*}}}*/

	//Traits for container type
	/*{{{*/

#ifdef ENABLE_NVML
	template <class Type>
		struct Container_Traits
		{
			static std::string header_message(){return Type::header_message();}
			typedef typename Type::Data_Type Data_Type;
			typedef typename Type::Const_Iterator_Type Const_Iterator_Type;
			static void print(const Data_Type &a){
				Const_Iterator_Type iterator;
				std::cout<<"-----------------------------"<<"\n";
				std::cout<<header_message()<<"\n";
				std::cout<<"*****************************"<<"\n";
				for(iterator=a.begin();iterator!=a.end();++iterator)
					std::cout<<*iterator<<"\n";
				std::cout<<"*****************************"<<"\n";
			}
		};

	struct Power_Vector_Container
	{
		static std::string header_message(){return "Power database:";}
		typedef NVML_Energy<>::Power_DB_Type Data_Type;
		typedef NVML_Energy<>::Power_DB_Const_Iterator_Type Const_Iterator_Type;
	};

	struct Hp_Power_Vector_Container
	{
		static std::string header_message(){return "Hp Power database:";}
		typedef NVML_Energy<>::Hp_Power_DB_Type Data_Type;
		typedef NVML_Energy<>::Hp_Power_DB_Const_Iterator_Type Const_Iterator_Type;
	};

	struct Time_Vector_Container
	{
		static std::string header_message(){return "Time database:";}
		typedef NVML_Energy<>::Time_DB_Type Data_Type;
		typedef NVML_Energy<>::Time_DB_Const_Iterator_Type Const_Iterator_Type;
	};
	inline std::ostream &operator<<(std::ostream &out, NVML_Energy<>::Time_Unit ts)
	{
		out.precision(15);
		out<<ts.tv_sec*1e6+ts.tv_nsec/1e3;
		return out;
	}
#endif


	/*}}}*/


	/*}}}*/

	//group2: Native Measurement Library Initializer
	/*{{{*/
	/** @defgroup group2 Native Measurement Library Initializer
	 *  Take GPU energy measurement as an example, initialize NVML library. 
	 *  @{
	 */


	//!  Library Initializer Interface. 
	/*!
	  
	*/
	struct Environment_Init
	{
		virtual void init()=0;
	};

	//!  CPU Time Library Initializer. 
	/*!
	  
	*/
	struct CPU_Time_Environment_Init : public Environment_Init
	{
/*{{{*/
		private:
			DECLARE_CLASS_NAME("CPU_Time_Environment_Init");
		public:
			void init(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
			}
/*}}}*/
	};

#ifdef ENABLE_CUDA_TIME
	//!  GPU Time Library Initializer. 
	/*!
	  
	*/
	struct CUDA_Time_Environment_Init : public Environment_Init
	{
/*{{{*/
		private:
			DECLARE_CLASS_NAME("CUDA_Time_Environment_Init");
		public:
			void init(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				}
/*}}}*/
	};
#endif


#ifdef ENABLE_PCM
	//!  CPU Energy Library Initializer. 
	/*!
	  
	*/
	struct PCM_Energy_Environment_Init : public Environment_Init
	{
/*{{{*/
		private:
			DECLARE_CLASS_NAME("PCM_Energy_Environment_Init");
		public:
			void init(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
			}
/*}}}*/
	};
#endif

#ifdef ENABLE_NVML
	//!  GPU Energy Library Initializer. 
	/*!
	  
	*/
	struct NVML_Energy_Environment_Init : public Environment_Init
	{
/*{{{*/
		public:
			void init(){NVML_Manager::init_NVML();}
		private:
			//!  NVML library init and teardown. 
			/*!
			  It contains methods for init and teardown of NVML library,
			  the default constructor initialize the NVML library.
			  */
			class NVML_Manager
				/*{{{*/
			{

				public:
					/**
					 * Application only needs to call this function at startup,
					 * lazy initialization only when called.
					 */
					static void init_NVML()
					{
						static NVML_Manager instance;
					}
				private:
					DECLARE_CLASS_NAME("NVML_Manager")
						/**
						 * Default constructor, private, 
						 * applications are forbidden to instantiate an object. 
						 */
						NVML_Manager(){init();}
					/**
					 * Default constructor, private, 
					 * if one object is created by calling init_NVML(),
					 * the destructor will be called automatically 
					 * at the end of a program.
					 */
					~NVML_Manager(){teardown();}
					/**
					 * Copy constructor, override default one, and also private.
					 */
					NVML_Manager(const NVML_Manager&);
					/**
					 * Assignment overloading, override default one, and also private.
					 */
					inline void operator=(const NVML_Manager&);
					/**
					 * Initialize the NVML library. 
					 */
					void init()
					{
						nvmlReturn_t result;

						// Initialize NVML library
						result = nvmlInit();
						if (NVML_SUCCESS != result)
						{
							printf("Failed to initialize NVML: %s\n", 
									nvmlErrorString(result));

							printf("Press ENTER to continue...\n");
							getchar();
							exit(1);
						}
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("called");
#endif
					}
					/**
					 * Shut down the NVML library.
					 */
					void teardown()
					{
						nvmlReturn_t result;
						result = nvmlShutdown();
						if (NVML_SUCCESS != result)
						{
							printf("Failed to shutdown NVML: %s\n", 
									nvmlErrorString(result));

							printf("All done.\n");

							printf("Press ENTER to continue...\n");
							getchar();
							exit(1);
						}
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("called");
#endif
					}
			};
			/*}}}*/
	/*}}}*/
	};
#endif

#ifdef ENABLE_SYSTEM_ENERGY
	//!  System Energy Library Initializer. 
	/*!
	  
	*/
	struct System_Energy_Environment_Init : public Environment_Init
	{
/*{{{*/
		private:
			DECLARE_CLASS_NAME("PCM_Energy_Environment_Init");
		public:
			void init(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
			}
/*}}}*/
	};
#endif



	/** @} */ // end of group2
	/*}}}*/

	//group3: Measurement controller
	/*{{{*/
	/** @defgroup group3 Measurement controller
	 *  Encapsulate logic that use native measurement 
	 *  libraries to start, stop and calculate measurement samples.
	 *  
	 *  @{
	 */


	//!  Measurement Controller Interface. 
	/*!
	  
	*/
	struct Measurement_Controller
	{
		/**
		 *  \brief mark the start of a measurement phase/period.
		 *  
		 *  
		 */
		virtual void start()=0;
		/**
		 *  \brief mark the end of a measurement phase/period.
		 *  
		 *  
		 */
		virtual void stop()=0;
		/**
		 *  \brief calculate the metric value between start() and stop().
		 *  
		 *  
		 */
		virtual void calc()=0;
		/**
		 *  \brief Print the calculated metric value to standard output, 
		 *  requires an invocation of calc() already done.
		 *  
		 *  
		 */
		virtual void show_meter_reading() const =0;
	};

	//!  CPU Time Measurement Controller. 
	/*!
	  Internally it uses clock_gettime() etc.
	*/
	struct CPU_Time_Measurement_Controller : public Measurement_Controller
	{
		/*{{{*/
		public:
			CPU_Time_Measurement_Controller(){}
			CPU_Time_Measurement_Controller(
					CPU_Time::Time_Unit const &start_time_p,
					CPU_Time::Time_Unit const &stop_time_p):start_time(start_time_p),stop_time(stop_time_p){}
		private:
			DECLARE_CLASS_NAME("CPU_Time_Measurement_Controller");
			CPU_Time::Time_Unit start_time;
			CPU_Time::Time_Unit stop_time;
			CPU_Time::ResultType meter_reading;
		public:
			inline void init()
			{
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
			}
			inline void start(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

				//clock_gettime(CLOCK_MONOTONIC, &start_time);
				METERPU_TIME_MEASURE(&start_time);
			}
			inline void stop(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

				METERPU_TIME_MEASURE(&stop_time);
			}
			inline void calc(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				meter_reading=stop_time-start_time;

			}
			/**
			 *  \brief Get calculated metric value, require calc() to be
			 *  called already.
			 *  
			 *  
			 */
			inline CPU_Time::ResultType const &get_value() const
			{
				return meter_reading;
			}
			inline void show_meter_reading() const {
				std::cout<<"[CPU Time Meter] Time consumed is "<<get_value()<<" micro seconds."<<std::endl;
			}
			/*}}}*/
	};

#ifdef ENABLE_CUDA_TIME

	//!  CUDA-enabled GPU Time Measurement Controller. 
	/*!
	  It internally uses cudaEventCreate() etc.
	*/
	struct CUDA_Time_Measurement_Controller : public Measurement_Controller
	{
/*{{{*/
	private:
		cudaEvent_t start_time;
		cudaEvent_t stop_time;
		CUDA_Time::ResultType meter_reading;
	public:
		CUDA_Time_Measurement_Controller()
		{
			cudaEventCreate(&start_time);
			cudaEventCreate(&stop_time);
		}
		~CUDA_Time_Measurement_Controller()
		{
			cudaEventDestroy(start_time);
			cudaEventDestroy(stop_time);
		}
		inline void init()
		{
#if METERPU_VERBOSE >= 1
			PRINT_CLASS_FUNC_NAME("called");
#endif
		}
		inline void start(){cudaEventRecord(start_time);}
		inline void stop(){cudaEventRecord(stop_time);}
		inline void calc(){cudaEventSynchronize(stop_time);
			cudaEventElapsedTime(&meter_reading, start_time, stop_time);
			meter_reading*=1000;
		}
		inline void show_meter_reading() const {
				std::cout<<"[CUDA Time Meter] Time consumed is "<<get_value()<<" micro seconds."<<std::endl;
		}
		/**
		 *  \brief Get calculated metric value, require calc() to be
		 *  called already.
		 *  
		 *  
		 */
		inline CUDA_Time::ResultType const& get_value() const {return meter_reading;}
/*}}}*/
	};
#endif


#ifdef ENABLE_PCM
	//!  CPU and DRAME Energy Measurement Controller. 
	/*!
	  It internally uses Intel <a href="https://software.intel.com/en-us/articles/intel-performance-counter-monitor">PCM</a> library.
	*/
	struct PCM_Energy_Measurement_Controller : public Measurement_Controller
	{
		/*{{{*/
		private:
			DECLARE_CLASS_NAME("PCM_Energy_Measurement_Controller");
		public:
			inline void init(){}
			inline void start(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				before_sstate = getSystemCounterState();
			}
			inline void stop(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				after_sstate = getSystemCounterState();
			}
			inline void calc()
			{
				update_cpu_energy();
				update_dram_energy();
#if METERPU_VERBOSE >= 2
				PRINT_CLASS_FUNC_NAME_CONT<<"cpu_energy("<<cpu_energy<<")"<<std::endl;
				PRINT_CLASS_FUNC_NAME_CONT<<"dram_energy("<<dram_energy<<")"<<std::endl;
#endif
				meter_reading=(cpu_energy+dram_energy)*1000.0f;
			}
		/**
		 *  \brief Get calculated metric value, require calc() to be
		 *  called already.
		 *  
		 *  
		 */
			inline PCM_Energy::ResultType const &get_value() const {return meter_reading;}
			inline void show_meter_reading() const{
				std::cout<<"[CPU and DRAM Energy Meter] Energy consumed is: "<<get_value()<<" milli Joules."<<std::endl;
			}
			inline PCM_Energy::ResultType const &get_cpu_energy() const {return cpu_energy;}
			inline PCM_Energy::ResultType const &get_dram_energy() const {return dram_energy;}
		private:
			PCM_Energy::ResultType meter_reading;
			PCM_Energy::CPU_Energy_Type cpu_energy;
			PCM_Energy::DRAM_Energy_Type dram_energy;
			inline void update_cpu_energy(){cpu_energy=getConsumedJoules(before_sstate, after_sstate);}
			inline void update_dram_energy(){dram_energy=getDRAMConsumedJoules(before_sstate, after_sstate);}
		public:
			PCM_Energy_Measurement_Controller():pcm(PCM::getInstance()){
				if(pcm->program() != PCM::Success)
				{
					PRINT_CLASS_FUNC_NAME_CONT<<"PCM not initialized, exit.";
					exit(1);
				}
				pcm->cleanup();
			}
			~PCM_Energy_Measurement_Controller(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				pcm->cleanup();}
		private:
			PCM *pcm;
			SystemCounterState before_sstate;
			SystemCounterState after_sstate;
			/*}}}*/
	};
#endif

#ifdef ENABLE_NVML
	//!  CUDA-enabled GPU Energy Measurement Controller. 
	/*!
	  It internally use NVML <a href="http://developer.download.nvidia.com/assets/cuda/files/CUDADownloads/NVML/nvml.pdf" target="_blank">nvmlDeviceGetPowerUsage()</a>, note that not all CUDA-enabled GPU support this functionality,
	  please check your GPU specification.

	  To calculate the energy value based on power samples obtained by NVML,
	  it implement the techniques described in the following paper:

	  [1] Martin Burtscher, Ivan Zecena, and Ziliang Zong. 2014. <a href="http://dl.acm.org/citation.cfm?id=2576783" target="_blank">Measuring GPU Power with the K20 Built-in Sensor.</a> In Proceedings of Workshop on General Purpose Processing Using GPUs (GPGPU-7). ACM, New York, NY, USA, Pages 28 , 9 pages. DOI=10.1145/2576779.2576783 http://doi.acm.org/10.1145/2576779.2576783
	*/
	template<GPU_Device_Id_Type device_id_NVML_Energy_Measurement_Controller>
		struct NVML_Energy_Measurement_Controller : public Measurement_Controller
	{

		/*{{{*/
		private:
			DECLARE_CLASS_NAME("NVML_Energy_Measurement_Controller");
		public:

			//!  Initialize a GPU device. 
			/*!
			  This function make if possible to initialize different CUDA GPUs if many exists.
			*/
			inline void init(){
/*{{{*/
				nvml_energy_device_init.init();
				device=nvml_energy_device_init.get_device();
				sampling_thread_controller.set_device(device);
/*}}}*/
			}
		private:
			template<GPU_Device_Id_Type device_id_value=device_id_NVML_Energy_Measurement_Controller>
				struct NVML_Energy_Device_Init
				{
					/*{{{*/
					private:
						DECLARE_CLASS_NAME("NVML_Energy_Device_Init")
							//! Device id.
							/*!
							  By default, it is zero, the first GPU installed on the target system.
							  */
							GPU_Device_Id_Type device_id;
						//! Device handle.
						/*!
						  initialized by device_id.
						  */
						nvmlDevice_t device;
					public:
						/**
						 * Default way to initialize device number.
						 */
						NVML_Energy_Device_Init():device_id(device_id_value){}
						/**
						 * Customed way to initialize device number.
						 */
						void set_device_id(GPU_Device_Id_Type id){device_id=id;}
						/**
						 * Initialize the device by device_id.
						 */
						void init()
						{
							nvmlReturn_t result;

							// Get the device handle
							result = nvmlDeviceGetHandleByIndex(device_id, &device);

							if (NVML_SUCCESS != result)
							{
								printf("Failed to get handle for device %i: %s\n",
										device_id, nvmlErrorString(result));
								exit(1);
							}

#if METERPU_VERBOSE >= 1

							PRINT_CLASS_FUNC_NAME("called");
#endif
						}
						/**
						 * Expose to Energy measurement, not visible to abstration.
						 */
						const nvmlDevice_t &get_device() const{return device;}
						/*}}}*/
				};
			NVML_Energy_Device_Init<> nvml_energy_device_init;
			nvmlDevice_t device;
			typename NVML_Energy<>::Energy_Unit meter_reading;
			CPU_Time::Time_Unit start_time;
			CPU_Time::Time_Unit stop_time;
			inline const CPU_Time::Time_Unit& get_start_time() const {return start_time;}
			inline const CPU_Time::Time_Unit& get_stop_time() const {return stop_time;}
			inline void record_start_time(){
				METERPU_TIME_MEASURE(&start_time);
			}
			inline void record_stop_time(){
				METERPU_TIME_MEASURE(&stop_time);
			}
			inline CPU_Time::ResultType diff(){return stop_time-start_time;}

		public:
			inline void start(){
/*{{{*/
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

				//reset all states thus ready for next measurement
				meter_reading=0;

				sampling_thread_controller.start();

				record_start_time();
/*}}}*/
			}
			inline void stop(){
/*{{{*/
				record_stop_time();
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

				//After the power curve becomes flat again, extend the flat region by 1000 microseconds,
				//to ensure there are more than 1 samples obtained. Substract the energy for that region later.
				//usleep(1000);
				sampling_thread_controller.stop();

#if METERPU_VERBOSE >= 3
				Container_Traits<Power_Vector_Container>::print(sampling_thread_controller.get_power_db());
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
#endif
/*}}}*/
			}
			inline void calc(){
/*{{{*/
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				//persist_compensation_data();
#ifdef METERPU_SHOW_POWER_DATA_SIZE
				PRINT_CLASS_FUNC_NAME_CONT<<"power size: "<<sampling_thread_controller.get_power_db().size()<<std::endl;
#endif
				unsigned size=sampling_thread_controller.get_power_db().size();
				if(size < 10){
					std::cerr<<std::endl;
					std::cerr<<"[MeterPU warnings: ] Number of Power Samples ("<<size<<" samples obtained) too less!"<<std::endl;
					std::cerr<<"The accurary of the energy value calculated may exceed 5%!"<<std::endl;
					std::cerr<<"Possble reasons can be: the latency of the driver for the first run,"<<std::endl;
					std::cerr<<"the kernel synchronization is forgotten,"<<std::endl;
					std::cerr<<"Or the kernel execution time is too short (recommended runtime: more than 200ms)."<<std::endl;
					std::cerr<<std::endl;
				}

				switch(size)
				{
					case 0:
						meter_reading=0;
						break;
					case 1:
						//time: macro second, power: milli Watt
						//divided by 1e6, thus we get milli Joule
						meter_reading=( *sampling_thread_controller.get_power_db().begin() ) 
							* diff()/1e6;
						break;
					case 2:
						//power average multipled by time
						meter_reading=( *sampling_thread_controller.get_power_db().begin() +  
								*(sampling_thread_controller.get_power_db().end()-1) ) 
							* diff()/2e6;
						break;

					default:
						//Dump original power data
#ifdef METERPU_DUMP_DATA_FILE
						resetDir(); //TODO: calculate a path
						dumpTimeEvent();
						dumpOriginalPowerData();
#endif

						//Do the postprocessing, only need to manipulate the power vector, 
						//then everything later should just work

						//Remove power values that are too close
						//sampling_thread_controller.get_power_db() sampling_thread_controller.get_time_db
						removeRedundantSamplesByDistance(4);
						
						//Correct power values based on Butcher's equation
						//Can use mapoverlap functional programming here
						correctPowerSamplesByBurtscherApproach();
						fillPowerValuesAtStartAndEnd();

						//An optinal macro: dump power and time to file for visualization

						//Integrate all values
						//Substract the static energy when needed
						meter_reading=calTotalArea(); 

						//dump corrected power data
#ifdef METERPU_DUMP_DATA_FILE
						dumpCorrectedPowerData();
#endif
				}

				//Still need to call this method, it will reset the state of sampling_thread_controller for next measure.
				sampling_thread_controller.calc();
/*}}}*/
			}
		/**
		 *  \brief Get calculated metric value, require calc() to be
		 *  called already.
		 *  
		 *  
		 */
			inline NVML_Energy<>::Energy_Unit get_value() const{return meter_reading;}
			inline void show_meter_reading() const{ std::cout<<"[GPU Energy Meter] Energy consumed is: "<<get_value()<<" milli Joules."<<std::endl; }

#if defined(METERPU_TEST)
			void test()
			{
/*{{{*/
				//Common facility
				NVML_Energy<>::Time_Unit middle1,middle2,middle3,middle4,middle5;
				NVML_Energy<>::Time_Unit middle6,middle7,middle8,middle9,middle10;
				NVML_Energy<>::Time_Unit noise1,noise2,noise3;
				bool test_enable[]={0,0,0,0,0,1};

				/**********************************************************************************************/
				if(test_enable[0]){
/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					sleep(1);
					record_stop_time();



					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 0 sample..."<<std::endl;
					calc();
					std::cout<<"Expected:   "<<0<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
/*}}}*/
				}

				/**********************************************************************************************/
				if(test_enable[1]){
/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					METERPU_TIME_MEASURE(&middle1);
					sleep(1);
					record_stop_time();

					sampling_thread_controller.get_time_db_nonconst().push_back(middle1);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);


					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 1 sample..."<<std::endl;
					calc();
					std::cout<<"Expected:   "<<200<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
/*}}}*/
				}
				/**********************************************************************************************/

				if(test_enable[2]){
					/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					METERPU_TIME_MEASURE(&middle1);
					sleep(1);
					METERPU_TIME_MEASURE(&middle2);
					sleep(1);
					record_stop_time();

					sampling_thread_controller.get_time_db_nonconst().push_back(middle1);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);
					sampling_thread_controller.get_time_db_nonconst().push_back(middle2);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);


					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 2 sample..."<<std::endl;
					calc();
					std::cout<<"Expected:   "<<300<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;

					/*}}}*/
				}
				/**********************************************************************************************/

				if(test_enable[3]){
					/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					METERPU_TIME_MEASURE(&middle1);
					sleep(1);
					METERPU_TIME_MEASURE(&middle2);
					sleep(1);
					METERPU_TIME_MEASURE(&middle3);
					sleep(1);
					record_stop_time();

					sampling_thread_controller.get_time_db_nonconst().push_back(middle1);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);
					sampling_thread_controller.get_time_db_nonconst().push_back(middle2);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);
					sampling_thread_controller.get_time_db_nonconst().push_back(middle3);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);


					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 3 sample..."<<std::endl;
					calc();
					std::cout<<"Expected:   "<<400<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
					/*}}}*/
				}
				/**********************************************************************************************/

				if(test_enable[4]){
					/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					METERPU_TIME_MEASURE(&middle1);
					METERPU_TIME_MEASURE(&noise1);
					sleep(1);
					METERPU_TIME_MEASURE(&middle2);
					METERPU_TIME_MEASURE(&noise2);
					sleep(1);
					METERPU_TIME_MEASURE(&middle3);
					METERPU_TIME_MEASURE(&noise3);
					sleep(1);
					record_stop_time();

					sampling_thread_controller.get_time_db_nonconst().push_back(middle1);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise1);
					sampling_thread_controller.get_power_db_nonconst().push_back(101);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle2);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise2);
					sampling_thread_controller.get_power_db_nonconst().push_back(101);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle3);
					sampling_thread_controller.get_power_db_nonconst().push_back(100);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise3);
					sampling_thread_controller.get_power_db_nonconst().push_back(101);


					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 3 samples with 3 noises..."<<std::endl;
					calc();
					std::cout<<"Expected:   "<<400<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
					/*}}}*/
				}


				if(test_enable[5]){
					/*{{{*/
					sampling_thread_controller.get_time_db_nonconst().clear();
					sampling_thread_controller.get_power_db_nonconst().clear();
					record_start_time();
					sleep(1);
					METERPU_TIME_MEASURE(&middle1);
					METERPU_TIME_MEASURE(&noise1);
					sleep(1);
					METERPU_TIME_MEASURE(&middle2);
					METERPU_TIME_MEASURE(&noise2);
					sleep(1);
					METERPU_TIME_MEASURE(&middle3);
					METERPU_TIME_MEASURE(&noise3);
					sleep(1);
					METERPU_TIME_MEASURE(&middle4);
					sleep(1);
					METERPU_TIME_MEASURE(&middle5);
					sleep(1);
					METERPU_TIME_MEASURE(&middle6);
					sleep(1);
					METERPU_TIME_MEASURE(&middle7);
					sleep(1);
					METERPU_TIME_MEASURE(&middle8);
					sleep(1);
					METERPU_TIME_MEASURE(&middle9);
					sleep(1);
					METERPU_TIME_MEASURE(&middle10);
					sleep(1);
					record_stop_time();

					
						
						

					sampling_thread_controller.get_time_db_nonconst().push_back(middle1);
					sampling_thread_controller.get_power_db_nonconst().push_back(53981);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise1);
					sampling_thread_controller.get_power_db_nonconst().push_back(51);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle2);
					sampling_thread_controller.get_power_db_nonconst().push_back(50929);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise2);
					sampling_thread_controller.get_power_db_nonconst().push_back(81);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle3);
					sampling_thread_controller.get_power_db_nonconst().push_back(49879);

					sampling_thread_controller.get_time_db_nonconst().push_back(noise3);
					sampling_thread_controller.get_power_db_nonconst().push_back(101);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle4);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle5);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle6);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle7);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle8);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle9);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);

					sampling_thread_controller.get_time_db_nonconst().push_back(middle10);
					sampling_thread_controller.get_power_db_nonconst().push_back(49783);


					//test operator overloading
					std::cout<<"-----------------------------------------------------------"<<std::endl;
					std::cout<<"Test calc: 10 samples, more realistic scenarios ..."<<std::endl;
					calc();
					std::cout<<"Expected:   NA, check the database directly, if power changes drastically instead of gradually."<<std::endl;
					std::cout<<"Calculated: "<<get_value()<<std::endl;
					std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
					/*}}}*/
				}
				/**********************************************************************************************/
/*}}}*/
			}
#endif
		private:
			std::string path;
			inline void resetDir(){
				path="power_data-";
				std::vector<std::string> bash_results;
				if(!bash_exe("date +%F-%H-%M-%S", bash_results) ) {std::cout<<"Command date not executes!"<<std::endl;exit(1);}

				path+=*bash_results.begin();
				if ( system( ("mkdir "+path).c_str() ) ) {std::cout<<"Command mkdir not executes!"<<std::endl;exit(1);}
			}
			inline void dumpTimeEvent(){
				std::ofstream out( (path+"/time_event.csv").c_str() );
				if(!out){ std::cout<<"file not open!"<<std::endl;exit(1); }
				out<<"start_time_us,end_time_us"<<std::endl;
				out<<get_start_time()<<",";
				out<<get_stop_time()<<std::endl;
			}
			template <class T1, class T2>
			void dumpTwoVectors(const std::string &filename, const std::string &header, const T1 &keys, const T2 &values ) const {
				assert(keys.size() == values.size());
				std::ofstream out( filename.c_str() );
				if(!out){ std::cout<<"file not open!"<<std::endl;exit(1); }
				out<<header<<std::endl;

				typename T1::const_iterator it_keys;
				typename T2::const_iterator it_values;

				it_keys=keys.begin();
				it_values=values.begin();

				for(;it_keys!=keys.end();++it_keys,++it_values) {
					out<<*it_keys<<","<<*it_values<<std::endl;
				}
			
			}
			inline void dumpOriginalPowerData() const {
				dumpTwoVectors(path+"/original_power_data.csv", "time_us,power_mw", sampling_thread_controller.get_time_db(), sampling_thread_controller.get_power_db() );
			}
			inline void dumpCorrectedPowerData(){
				dumpTwoVectors(path+"/corrected_power_data.csv", "time_us,power_mw", sampling_thread_controller.get_time_db(), correctedPowerDB );
			}
			void removeRedundantSamplesByDistance(const CPU_Time::ResultType& time_distance_ms){
/*{{{*/

#ifdef METERPU_DEBUG_NVML_CALC
				std::cout<<"original databases: "<<std::endl;
				Container_Traits<Power_Vector_Container>::print(sampling_thread_controller.get_power_db());
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
				std::cout<<std::endl;
#endif

				const CPU_Time::ResultType time_distance_us=time_distance_ms*1e3;
				//First mark the positions of non-redundant sample
				//Then add those samples to a new vector
				//Delete the redundant ones directly is not a
				//good idea, iterator will change, and
				//delete data in vector in general not good
				std::vector<bool> nonRedundantSampleFlag(sampling_thread_controller.get_power_db().size(), 0);
				//The first sample will always be non-redundant
				nonRedundantSampleFlag[0]=1;

				NVML_Energy<>::Time_DB_Type::const_iterator it=sampling_thread_controller.get_time_db().begin()+1;
				int index=1;

				//mark all elements except the first one
				while(it!=sampling_thread_controller.get_time_db().end()){
					if( *it - *(it-1) >= time_distance_us ){
						nonRedundantSampleFlag[index]=1;
					}
					++index;
					++it;
				}

				//make new vectors only contain nonredundant ones
				NVML_Energy<>::Time_DB_Type nonRedundantTimeDB;
				NVML_Energy<>::Power_DB_Type nonRedundantPowerDB;

				//placeholder at the starting position for time db
				nonRedundantTimeDB.push_back( NVML_Energy<>::Time_Unit() );


				index=0;
				while( index<nonRedundantSampleFlag.size() ) {
					if(nonRedundantSampleFlag[index]){
						nonRedundantTimeDB.push_back(sampling_thread_controller.get_time_db()[index]);
						nonRedundantPowerDB.push_back(sampling_thread_controller.get_power_db()[index]);
					}
					++index;
				}

				//do the assignment
				sampling_thread_controller.set_time_db(nonRedundantTimeDB);
				sampling_thread_controller.set_power_db(nonRedundantPowerDB);

				
#ifdef METERPU_DEBUG_NVML_CALC
				std::cout<<"After remove redundant samples"<<std::endl;
				Container_Traits<Power_Vector_Container>::print(sampling_thread_controller.get_power_db());
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
				std::cout<<std::endl;
#endif

				/*}}}*/
			}
			NVML_Energy<>::Hp_Power_DB_Type correctedPowerDB;
			void correctPowerSamplesByBurtscherApproach(){
/*{{{*/
				//P_true (t_i ) = P_meas (t_i ) + C × (P_meas (t_i+1 ) - P_meas (t_i-1 )) / (t_i+1 - t_i-1)
				//The first one and last one are harder to calculate,
				//since you don't have all data available,
				//placeholder at starting position for powerdb
				correctedPowerDB.assign(sampling_thread_controller.get_power_db().size()+1,0);
				const float C = 0.84f;
				//start with the second power samples
				//corrected PowerDB: 0 x x ... //start with 2
				//meas PowerDB: x x ... //start with 1
				//timedb: 0 x x
				int index_placeholder=2, index=1;
				while( index < sampling_thread_controller.get_power_db().size() - 1 ) {
					//Only for debugging
					/*{{{*/
#ifdef NEVER_DEFINE_IT_UNTIL_YOU_REALLY_WANT
					if(index==15){
						std::cout<<( (float)sampling_thread_controller.get_power_db()[index+1] - (float)sampling_thread_controller.get_power_db()[index-1])<<std::endl;
						std::cout<<(sampling_thread_controller.get_time_db()[index_placeholder+1] - sampling_thread_controller.get_time_db()[index_placeholder-1])/1e6<<std::endl;
						std::cout<<( (float)sampling_thread_controller.get_power_db()[index+1] - (float)sampling_thread_controller.get_power_db()[index-1])/((sampling_thread_controller.get_time_db()[index_placeholder+1] - sampling_thread_controller.get_time_db()[index_placeholder-1])/1e6)<<std::endl;
						std::cout<<C*( (float)sampling_thread_controller.get_power_db()[index+1] - (float)sampling_thread_controller.get_power_db()[index-1])/((sampling_thread_controller.get_time_db()[index_placeholder+1] - sampling_thread_controller.get_time_db()[index_placeholder-1])/1e6)<<std::endl;
						std::cout<<sampling_thread_controller.get_power_db()[index]+C*( (float)sampling_thread_controller.get_power_db()[index+1] - (float)sampling_thread_controller.get_power_db()[index-1])/((sampling_thread_controller.get_time_db()[index_placeholder+1] - sampling_thread_controller.get_time_db()[index_placeholder-1])/1e6)<<std::endl;
					}
#endif
/*}}}*/
					correctedPowerDB[index_placeholder] =
						sampling_thread_controller.get_power_db()[index] +
						C * ( (float)sampling_thread_controller.get_power_db()[index+1] 
						- (float)sampling_thread_controller.get_power_db()[index-1] ) / 
						( (sampling_thread_controller.get_time_db()[index_placeholder+1]
						 - sampling_thread_controller.get_time_db()[index_placeholder-1]) /1e6 );
					++index;
					++index_placeholder;
				}

				//Now original data is not useful
				sampling_thread_controller.get_power_db_nonconst().clear();
				
				//Make the first and last be the same as the
				//second and the second last as an approximation
				correctedPowerDB[1]
					= correctedPowerDB[2];
				*(correctedPowerDB.end() - 1)
					= *(correctedPowerDB.end() - 2);

#ifdef METERPU_DEBUG_NVML_CALC
				std::cout<<"After correcting power samples"<<std::endl;
				//Container_Traits<Power_Vector_Container>::print(sampling_thread_controller.get_power_db());
				Container_Traits<Hp_Power_Vector_Container>::print(correctedPowerDB);
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
				std::cout<<std::endl;
#endif
				/*}}}*/
			}
			void fillPowerValuesAtStartAndEnd(){
/*{{{*/
				//Fill the power value at kernel start time
				//*(sampling_thread_controller.get_power_db_nonconst().begin() )
					//= *(sampling_thread_controller.get_power_db().begin() + 1);
				correctedPowerDB[0]=correctedPowerDB[1];
				//Fill the time value at kernel start time
				*sampling_thread_controller.get_time_db_nonconst().begin() = 
					get_start_time() < *(sampling_thread_controller.get_time_db().begin()+1)?
					get_start_time():*(sampling_thread_controller.get_time_db().begin()+1);


				if( *(sampling_thread_controller.get_time_db().end()-1) < get_stop_time() ) {
					//Fill the time value at kernel end time
					sampling_thread_controller.get_time_db_nonconst().push_back( get_stop_time() );
					//Fill the power value at kernel end time
					//sampling_thread_controller.get_power_db_nonconst().push_back(
							//*(sampling_thread_controller.get_power_db().end() - 1)
							//);
					correctedPowerDB.push_back( *(correctedPowerDB.end()-1) );
				}
#ifdef METERPU_DEBUG_NVML_CALC
				std::cout<<"After filling values at start and end"<<std::endl;
				Container_Traits<Hp_Power_Vector_Container>::print(correctedPowerDB);
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
				std::cout<<std::endl;
#endif
/*}}}*/
			}
		private:
			
			 
			NVML_Energy<>::Energy_Unit calTotalArea()
			{
/*{{{*/
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				return calTotalArea(correctedPowerDB,sampling_thread_controller.get_time_db());
/*}}}*/
			}
			NVML_Energy<>::Energy_Unit calTotalArea(
					NVML_Energy<>::Hp_Power_DB_Type const &power_db,
					NVML_Energy<>::Time_DB_Type const &time_db
					)
			{
/*{{{*/
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				NVML_Energy<>::Energy_Unit totalArea=0;
				assert(power_db.size()==time_db.size());
				NVML_Energy<>::Hp_Power_DB_Type::const_iterator power_iter=power_db.begin();
				NVML_Energy<>::Time_DB_Type::const_iterator time_iter=time_db.begin();
				for(;power_iter!=power_db.end()-1;)
				{
					totalArea+=calAreaForOneTrapezoid(*time_iter,*(time_iter+1),*(power_iter),*(power_iter+1));
					++power_iter;
					++time_iter;
				}
				//Since time unit is micro second, here we convert it to second,
				//power unit is already milliwatts, thus we get milli Joule.
				return totalArea/1000000.0f;
/*}}}*/
			}
			NVML_Energy<>::Energy_Unit calAreaForOneTrapezoid(
					NVML_Energy<>::Time_Unit start_time,
					NVML_Energy<>::Time_Unit end_time,
					NVML_Energy<>::Hp_Power_Unit start_power,
					NVML_Energy<>::Hp_Power_Unit end_power
					)
			{
/*{{{*/
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				NVML_Energy<>::Hp_Power_Unit h=(start_power>end_power?start_power:end_power);
				NVML_Energy<>::Hp_Power_Unit l=(start_power<=end_power?start_power:end_power);
				CPU_Time::ResultType r=end_time-start_time;
#ifdef TEST_ENERGY_CAL
				std::cout<<(l*r+(h-l)*r/2)/1000000.0f<<std::endl;
#endif
				return l*r+(h-l)*r/2;
/*}}}*/
			}
			//!  Sampling Thread Controller. 
			/*!
			  Start a thread sampling power samples, and wait for stop signal.
			*/
			class Sampling_Thread_Controller 
				: public Measurement_Controller
			{
				/*{{{*/
				public:
					Sampling_Thread_Controller():power(get_UNREALISTIC_POWER_VALUE()){}
				private:
					DECLARE_CLASS_NAME("Sampling_Thread_Controller");
					static NVML_Energy<>::Power_Unit get_UNREALISTIC_POWER_VALUE(){ return (NVML_Energy<>::Power_Unit)100000000;}
					bool sampling;
					//! Device handle.
					/*!
					  
					  */
					nvmlDevice_t device;
					//! Power value for each sample.
					/*!
					  
					  */
					NVML_Energy<>::Power_Unit power;
					//! Time value for each sample.
					/*!
					  */
					NVML_Energy<>::Time_Unit time;
					//! Power data database.
					/*!
					  Store time and power value pairs.
					  */
					NVML_Energy<>::Time_DB_Type time_db;
					NVML_Energy<>::Power_DB_Type power_db;
					static void *thread_program(void *arg)
					{
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("start");
#endif

						bool * const sampling_local=&((Sampling_Thread_Controller *)arg)->sampling;
						NVML_Energy<>::Time_Unit * const time_local=&((Sampling_Thread_Controller *)arg)->time;
						NVML_Energy<>::Power_Unit * const power_local=&((Sampling_Thread_Controller *)arg)->power;
						NVML_Energy<>::Time_DB_Type * const time_db_local=&((Sampling_Thread_Controller *)arg)->time_db;
						NVML_Energy<>::Power_DB_Type * const power_db_local=&((Sampling_Thread_Controller *)arg)->power_db;
						nvmlDevice_t const device_local=((Sampling_Thread_Controller *)arg)->device;

						nvmlReturn_t return_code; 
#if METERPU_VERBOSE >= 2
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling_local("<<*sampling_local<<")\n";
#endif

						while(*sampling_local)
						{

							//Sampling power
							return_code = nvmlDeviceGetPowerUsage( device_local, power_local );
							if ( return_code != NVML_SUCCESS) { 
								continue;
							}
							METERPU_TIME_MEASURE(time_local);

#if METERPU_VERBOSE >= 4
							PRINT_CLASS_FUNC_NAME_CONT<<"current power("<<*power_local<<")\n";
#endif

							//Add time and power pair to database
							power_db_local->push_back(*power_local);
							time_db_local->push_back(*time_local);
						}

#if METERPU_VERBOSE >= 2
						NVML_Energy<>::Time_DB_Type::iterator it=time_db_local->end();
						PRINT_CLASS_FUNC_NAME_CONT<<"Obtained stop signal, the last time stamp of sampling is: "
							<<*--it
							<<std::endl;
#endif

						pthread_exit((void*) arg);
					}
					pthread_t thread;
					pthread_attr_t attr;
				public:
					void start()
					{
						int rc;

						pthread_attr_init(&attr);
						pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

						//Set the thread runnable flag
						sampling=true;

#if METERPU_VERBOSE >= 2
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling("<<sampling<<")\n";
#endif

						//Release the thread
						rc = pthread_create(&thread, &attr, thread_program, (void *)this); 
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("thread released");
#endif
						if (rc) {
							printf("ERROR; return code from pthread_create() is %d\n", rc);
							exit(-1);
						}
					}
					void stop(){
						int rc;
						void *status;

						//Set stop signal
						sampling=false;

						//Wait for thread to join
						rc = pthread_join(thread, &status);
						if (rc) {
							printf("ERROR; return code from pthread_join() is %d\n", rc);
							exit(-1);
						}
						pthread_attr_destroy(&attr);

#if METERPU_VERBOSE >= 1

						NVML_Energy<>::Time_DB_Type::iterator it=time_db.end();
						CPU_Time_Measurement_Controller temp(*time_db.begin(),*--it);
						temp.calc();
						PRINT_CLASS_FUNC_NAME_CONT<<"time period of sampling is: "
							<<temp.get_value()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"number of sampling is: "
							<<power_db.size()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"The first sample of power is: "
							<<*power_db.begin()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling rate is: "
							<<power_db.size()/temp.get_value()
							<<" #samples/sec."
							<<std::endl;
#endif


					}
					void calc(){
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("called");
#endif

						//reset state for next measurement
						reset_state();

					}
					void show_meter_reading() const {PRINT_FUNC_NAME_CONT<<"Not applicable to call me..."<<std::endl;}
					void set_device(const nvmlDevice_t &device_handle){device=device_handle;}
					const NVML_Energy<>::Power_Unit &get_power() const {return power;}
					const NVML_Energy<>::Power_DB_Type &get_power_db() const {return power_db;}
					const NVML_Energy<>::Time_DB_Type &get_time_db() const {return time_db;}
					NVML_Energy<>::Power_DB_Type &get_power_db_nonconst() {return power_db;}
					NVML_Energy<>::Time_DB_Type &get_time_db_nonconst() {return time_db;}
					void set_time_db(const NVML_Energy<>::Time_DB_Type &x){time_db=x;}
					void set_power_db(const NVML_Energy<>::Power_DB_Type &x){power_db=x;}
					NVML_Energy<>::Time_Unit const &get_stop_time(){
						assert(time_db.size()>=1);
						NVML_Energy<>::Time_DB_Type::iterator it=time_db.end();
						return *--it;
					}
					NVML_Energy<>::Time_Unit const &get_start_time(){
						assert(time_db.size()>=1);
						return *time_db.begin();
					}
					void reset_state(){time_db.clear();power_db.clear();}

					/*}}}*/
			};
			Sampling_Thread_Controller sampling_thread_controller;
		/*}}}*/
	};

#endif


//Obsolete
#ifdef ENABLE_NVML_OBSOLETE
	template<GPU_Device_Id_Type device_id_NVML_Energy_Measurement_Controller>
		struct NVML_Energy_Measurement_Controller : public Measurement_Controller
	{
		/*{{{*/
		private:
			DECLARE_CLASS_NAME("NVML_Energy_Measurement_Controller");
		public:
			void init(){
				nvml_energy_device_init.init();
				device=nvml_energy_device_init.get_device();
				sampling_thread_controller.set_device(device);
			}
		private:
			template<GPU_Device_Id_Type device_id_value=device_id_NVML_Energy_Measurement_Controller>
				struct NVML_Energy_Device_Init
				{
					/*{{{*/
					private:
						DECLARE_CLASS_NAME("NVML_Energy_Device_Init")
							//! Device id.
							/*!
							  By default, it is zero, the first GPU installed on the target system.
							  */
							GPU_Device_Id_Type device_id;
						//! Device handle.
						/*!
						  initialized by device_id.
						  */
						nvmlDevice_t device;
					public:
						/**
						 * Default way to initialize device number.
						 */
						NVML_Energy_Device_Init():device_id(device_id_value){}
						/**
						 * Customed way to initialize device number.
						 */
						void set_device_id(GPU_Device_Id_Type id){device_id=id;}
						/**
						 * Initialize the device by device_id.
						 */
						void init()
						{
							nvmlReturn_t result;

							// Get the device handle
							result = nvmlDeviceGetHandleByIndex(device_id, &device);

							if (NVML_SUCCESS != result)
							{
								printf("Failed to get handle for device %i: %s\n",
										device_id, nvmlErrorString(result));
								exit(1);
							}

#if METERPU_VERBOSE >= 1

							PRINT_CLASS_FUNC_NAME("called");
#endif
						}
						/**
						 * Expose to Energy measurement, not visible to abstration.
						 */
						const nvmlDevice_t &get_device() const{return device;}
						/*}}}*/
				};
			NVML_Energy_Device_Init<> nvml_energy_device_init;
			nvmlDevice_t device;
			NVML_Energy<>::Time_Unit execution_stop_time;
			typename NVML_Energy<>::Energy_Unit meter_reading;
		public:
#ifdef METERPU_TEST
			void test()
				/*{{{*/
			{
				//Common facility
				NVML_Energy<>::Time_Unit start_time, stop_time, compensation_end;
				//clock_gettime(CLOCK_MONOTONIC, &start_time);
				METERPU_TIME_MEASURE(&start_time);
				sleep(1);
				//clock_gettime(CLOCK_MONOTONIC, &stop_time);
				METERPU_TIME_MEASURE(&stop_time);
				sleep(1);
				//clock_gettime(CLOCK_MONOTONIC, &compensation_end);
				METERPU_TIME_MEASURE(&compensation_end);


				//test operator overloading
				std::cout<<"-----------------------------------------------------------"<<std::endl;
				std::cout<<"Test operator overloading..."<<std::endl;
				std::cout<<"Expected:   "<<1e6<<std::endl;
				std::cout<<"Calculated: "<<stop_time-start_time<<std::endl;
				std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
				//A failure case
				//std::cout<<start_time-stop_time<<std::endl;

				//test calAreaForOneTrapezoid()
				std::cout<<"-----------------------------------------------------------"<<std::endl;
				std::cout<<"Test calAreaForOneTrapezoid()..."<<std::endl;
				std::cout<<"Expected:   "<<1.5<<std::endl;
				std::cout<<"Calculated: "<<calAreaForOneTrapezoid(start_time,stop_time,1,2)<<std::endl;
				std::cout<<"Expected:   "<<1.5<<std::endl;
				std::cout<<"Calculated: "<<calAreaForOneTrapezoid(start_time,stop_time,2,1)<<std::endl;
				std::cout<<"Expected:   "<<1<<std::endl;
				std::cout<<"Calculated: "<<calAreaForOneTrapezoid(start_time,stop_time,1,1)<<std::endl;
				std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;

				//test calTotalArea()
				std::cout<<"-----------------------------------------------------------"<<std::endl;
				std::cout<<"Test calTotalArea()..."<<std::endl;
				NVML_Energy<>::Power_DB_Type power_db;
				power_db.push_back(1);
				power_db.push_back(2);
				power_db.push_back(3);

				NVML_Energy<>::Time_DB_Type time_db;
				time_db.push_back(start_time);
				time_db.push_back(stop_time);
				time_db.push_back(compensation_end);

				std::cout<<"Expected:   "<<4<<std::endl;
				std::cout<<"Calculated: "<<calTotalArea(power_db,time_db)<<std::endl;
				std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;

				//test if energy calculation is correct by cooperation of calTotalArea(), calCompensatedStaticEnergy()
				//and calStaticEnergy()
				std::cout<<"-----------------------------------------------------------"<<std::endl;
				std::cout<<"Test energy calculation by"
					<<" calTotalArea(), calCompensatedStaticEnergy() and calStaticEnergy()..."<<std::endl;
				std::cout<<"Expected:   "<<3<<std::endl;
				std::cout<<"Calculated: "
					<<calTotalArea(power_db,time_db)-calCompensatedStaticEnergy(stop_time,compensation_end,1)<<std::endl;
				std::cout<<"Expected:   "<<2<<std::endl;
				std::cout<<"Calculated: "
					<<calTotalArea(power_db,time_db)-calCompensatedStaticEnergy(stop_time,compensation_end,1)-calStaticEnergy(start_time,stop_time,1)<<std::endl;
				std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;

				//test time unit comparison
				std::cout<<"-----------------------------------------------------------"<<std::endl;
				std::cout<<"Test time unit comparison"<<std::endl;
				std::cout<<std::boolalpha;
				CPU_Time::Time_Unit small, large;
				//Equal case
				small.tv_sec=1;small.tv_nsec=2;
				large.tv_sec=1;large.tv_nsec=2;
				std::cout<<"1 Expected: equal true"<<std::endl;
				std::cout<<"1  Calculated: "<<(small==large)<<std::endl;
				std::cout<<"2 Expected: small and equal true"<<std::endl;
				std::cout<<"2 Calculated: "<<(small<=large)<<std::endl;
				//First small
				small.tv_sec=2;small.tv_nsec=2;
				large.tv_sec=3;large.tv_nsec=2;
				std::cout<<"3 Expected: small true"<<std::endl;
				std::cout<<"3 Calculated: "<<(small<large)<<std::endl;
				std::cout<<"4 Expected: small and equal true"<<std::endl;
				std::cout<<"4 Calculated: "<<(small<=large)<<std::endl;
				//Second small
				small.tv_sec=1;small.tv_nsec=1;
				large.tv_sec=1;large.tv_nsec=2;
				std::cout<<"5 Expected: small true"<<std::endl;
				std::cout<<"5 Calculated: "<<(small<large)<<std::endl;
				std::cout<<"6 Expected: small and equal true"<<std::endl;
				std::cout<<"6 Calculated: "<<(small<=large)<<std::endl;
				//Fisrt small, second large
				small.tv_sec=0;small.tv_nsec=4;
				large.tv_sec=1;large.tv_nsec=2;
				std::cout<<"7 Expected: small true"<<std::endl;
				std::cout<<"7 Calculated: "<<(small<large)<<std::endl;
				std::cout<<"8 Expected: small and equal true"<<std::endl;
				std::cout<<"8 Calculated: "<<(small<=large)<<std::endl;
				//Fisrt large, second small
				small.tv_sec=2;small.tv_nsec=1;
				large.tv_sec=1;large.tv_nsec=2;
				std::cout<<"9 Expected: small false"<<std::endl;
				std::cout<<"9 Calculated: "<<(small<large)<<std::endl;
				std::cout<<"10 Expected: small and equal false"<<std::endl;
				std::cout<<"10 Calculated: "<<(small<=large)<<std::endl;
				std::cout<<"-----------------------------------------------------------"<<std::endl<<std::endl;
			}
			/*}}}*/
#endif
			/**
			 * 1) Sample the static power consumption, which can be used later
			 * for termination of sampling thread
			 * 2) creation of sampling thread. 
			 * The sampling threads will waits for termination signal,
			 * otherwise it will sample the power value.
			 */
			void start(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

				//reset all states thus ready for next measurement
				meter_reading=0;

				sampling_static_power();
				sampling_thread_controller.start();
			}

			/**
			 * this function knows when function under measurement finishes,
			 * thus it is time to watch the power value to drop down,
			 * until it finishes.
			 */
			void stop(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif

#if METERPU_VERBOSE >= 2
				PRINT_CLASS_FUNC_NAME("mark down the execution stop time");
#endif
				//mark the execution stop time
				//clock_gettime(CLOCK_MONOTONIC, &execution_stop_time);
				METERPU_TIME_MEASURE(&execution_stop_time);
#if METERPU_VERBOSE >= 2
				PRINT_CLASS_FUNC_NAME_CONT<<"time stamp of execution time: ("<<execution_stop_time<<")"<<std::endl;
#endif

				typename NVML_Energy<>::Hp_Power_Unit power_threshold
					= //(typename NVML_Energy<>::Power_Unit)
					(static_power_value*1.05f);


				do{
#if METERPU_VERBOSE >= 3
					PRINT_CLASS_FUNC_NAME_CONT<<"waiting to release stop signal: power("<<sampling_thread_controller.get_power()<<")\n";
#endif
				}while(sampling_thread_controller.get_power()>power_threshold);
#if METERPU_VERBOSE >= 2
				PRINT_CLASS_FUNC_NAME_CONT<<"power_threshold("<<power_threshold<<")\n";
				PRINT_CLASS_FUNC_NAME_CONT<<"about to release stop signal: power("<<sampling_thread_controller.get_power()<<")\n";
#endif
				//After the power curve becomes flat again, extend the flat region by 1000 microseconds,
				//to ensure there are more than 1 samples obtained. Substract the energy for that region later.
				//usleep(1000);
				sampling_thread_controller.stop();

#if METERPU_VERBOSE >= 3
				Container_Traits<Power_Vector_Container>::print(sampling_thread_controller.get_power_db());
				Container_Traits<Time_Vector_Container>::print(sampling_thread_controller.get_time_db());
#endif



			}
			void calc(){
#if METERPU_VERBOSE >= 1
				PRINT_CLASS_FUNC_NAME("called");
#endif
				//persist_compensation_data();
#ifdef DEBUG_OPERATOR_MINUS
				PRINT_CLASS_FUNC_NAME_CONT<<"power size: "<<sampling_thread_controller.get_power_db().size()<<std::endl;
#endif

				//Do the calculation here, and assign them to meter_reading,
				//offer two possibilities, whole energy and dynamic energy.
				switch(sampling_thread_controller.get_power_db().size())
				{
					case 0:
						meter_reading=0;
						break;
					case 1:
						//The energy estimated here is the half of the energy that minimum time needed 
						//to obtain two samples when execute nothing.
						//which is 5.241385
						meter_reading=5.241385;
						break;
					default:
						//Substract the static energy when needed
						meter_reading=calTotalArea()-calCompensatedStaticEnergy();//-calStaticEnergy();
				}

				//Still need to call this method, it will reset the state of sampling_thread_controller for next measure.
				sampling_thread_controller.calc();

				//Let the GPU cool down for 10 seconds to mitigate temperature effect.
				//sleep(1);

			}
			NVML_Energy<>::Energy_Unit get_value() const{return meter_reading;}
			void show_meter_reading() const{
				std::cout<<"Energy consumed is: "<<get_value()<<" milli Joules."<<std::endl;
			}
		private:
			/**
			 * Calculate the whole area under the curve, bind parameters
			 */
			NVML_Energy<>::Energy_Unit calTotalArea()
			{
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				return calTotalArea(sampling_thread_controller.get_power_db(),sampling_thread_controller.get_time_db());
			}
			/**
			 * General component
			 */
			NVML_Energy<>::Energy_Unit calTotalArea(
					NVML_Energy<>::Power_DB_Type const &power_db,
					NVML_Energy<>::Time_DB_Type const &time_db
					)
			{
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				NVML_Energy<>::Energy_Unit totalArea=0;
				//std::cout<<std::endl<<power_db.size()<<std::endl;
				assert(power_db.size()==time_db.size());
				NVML_Energy<>::Power_DB_Type::const_iterator power_iter=power_db.begin();
				NVML_Energy<>::Time_DB_Type::const_iterator time_iter=time_db.begin();
				for(;power_iter!=power_db.end()-1;)
				{
					totalArea+=calAreaForOneTrapezoid(*time_iter,*(time_iter+1),*(power_iter),*(power_iter+1));
					++power_iter;
					++time_iter;
				}
				//Since time unit is micro second, here we convert it to second,
				//power unit is already milliwatts, thus we get milli Joule.
				return totalArea/1000000.0f;
			}
			/**
			 * Calculate the trapezoid area for one interval
			 */
			NVML_Energy<>::Energy_Unit calAreaForOneTrapezoid(
					NVML_Energy<>::Time_Unit start_time,
					NVML_Energy<>::Time_Unit end_time,
					NVML_Energy<>::Power_Unit start_power,
					NVML_Energy<>::Power_Unit end_power
					)
			{
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				NVML_Energy<>::Power_Unit h=(start_power>end_power?start_power:end_power);
				NVML_Energy<>::Power_Unit l=(start_power<=end_power?start_power:end_power);
				CPU_Time::ResultType r=end_time-start_time;
#ifdef TEST_ENERGY_CAL
				//PRINT_CLASS_FUNC_NAME_CONT<<"AreaForOneTrapezoid("<<l*r+(h-l)*r/2<<")"<<std::endl;
				std::cout<<(l*r+(h-l)*r/2)/1000000.0f<<std::endl;
#endif
				return l*r+(h-l)*r/2;
			}
			/**
			 * Calculate the static energy in compensated time period 
			 * (between execution ends and the time stamp power stops to static power value)
			 */
			NVML_Energy<>::Energy_Unit calCompensatedStaticEnergy()
			{
#if METERPU_VERBOSE >= 2 || defined(DEBUG_OPERATOR_MINUS)
				PRINT_CLASS_FUNC_NAME("called");
#endif
				return calCompensatedStaticEnergy(execution_stop_time,sampling_thread_controller.get_stop_time(),static_power_value);
			}
			/**
			 * General component: calculate the static energy in compensated time period 
			 * (between execution ends and the time stamp power stops to static power value)
			 */
			NVML_Energy<>::Energy_Unit calCompensatedStaticEnergy(
					NVML_Energy<>::Time_Unit execution_end,
					NVML_Energy<>::Time_Unit compensation_end,
					NVML_Energy<>::Power_Unit static_power_value
					)
			{
				CPU_Time::ResultType compensation_period=(execution_end<compensation_end?compensation_end-execution_end:0);
#if METERPU_VERBOSE>=2 || defined TEST_ENERGY_CAL || defined DEBUG_OPERATOR_MINUS
				PRINT_CLASS_FUNC_NAME_CONT<<" compensation_end: "<<compensation_end<<std::endl;
				PRINT_CLASS_FUNC_NAME_CONT<<" execution_end: "<<execution_end<<std::endl;
				PRINT_CLASS_FUNC_NAME_CONT<<"compensation_period("<<(compensation_end-execution_end)/1000000.0f<<")"<<std::endl;

#endif
				assert(compensation_period>=0);
				return (compensation_period)*static_power_value/1000000.0f;
			}
			/**
			 * Calculate the static energy in GPU component execution time window.
			 */
			NVML_Energy<>::Energy_Unit calStaticEnergy()
			{
				return calStaticEnergy(sampling_thread_controller.get_start_time(),execution_stop_time,static_power_value);
			}
			/**
			 * General component: Calculate the static energy in GPU component execution time window.
			 */
			NVML_Energy<>::Energy_Unit calStaticEnergy(
					NVML_Energy<>::Time_Unit execution_start,
					NVML_Energy<>::Time_Unit execution_end,
					NVML_Energy<>::Power_Unit static_power_value
					)
			{
				return (execution_end-execution_start)*static_power_value/1000000.0f;
			}

			//lu/  TODO: might be a good place to apply smart sampling by std_dev
			void sampling_static_power(){
				/*{{{*/
				nvmlReturn_t return_code; 

				static_power_value=0;
				int valid_power_sampling_count=0;
				const int num_of_samples=10;

				for(int i=0;i<num_of_samples;++i){	
					//Sampling power
					return_code = nvmlDeviceGetPowerUsage( device, &power );
					if ( return_code != NVML_SUCCESS) { 
#if METERPU_VERBOSE >= 3
						PRINT_CLASS_FUNC_NAME("warnings(failed in getting one power sample value)");
#endif
						--i;
						continue;
					}

#if METERPU_VERBOSE >= 3
					PRINT_CLASS_FUNC_NAME_CONT<<"current sample value("<<power<<")\n";
#endif

					valid_power_sampling_count++;
					static_power_value+=power;
				}
#if METERPU_VERBOSE >= 2 || defined TEST_ENERGY_CAL
				PRINT_CLASS_FUNC_NAME_CONT<<"Sum of static power value("<<static_power_value<<")\n";
				PRINT_CLASS_FUNC_NAME_CONT<<"valid_power_sampling_count("<<valid_power_sampling_count<<")\n";
#endif
				static_power_value=static_power_value/valid_power_sampling_count;
#if METERPU_VERBOSE >= 2
				std::cout<<std::setprecision(10)<<"sampling_static_power(): final static power("<<static_power_value<<")\n";
#endif
				/*}}}*/
			}
			//! Energy data type
			/*! Unit: mW (MiliWatt).
			*/
			NVML_Energy<>::Hp_Power_Unit static_power_value;
			NVML_Energy<>::Power_Unit power;
			//void persist_compensation_data(){

			//#if METERPU_VERBOSE >= 1
			//PRINT_CLASS_FUNC_NAME("called");
			//#endif

			////write time range between execution ends and sampling ends, and also the static power value.
			////format: "diff_tvsec","diff_tvnsec","static_power"
			////	   1, 0, 1
			//std::ofstream out1("appgen_energy_compensation.csv");
			//if(!out1){
			//std::cout<<"File creation error!"<<std::endl;
			//exit(1);
			//}
			//out1<<"\"diff_tvsec\",\"diff_tvnsec\",\"static_power\""<<std::endl;

			//#if METERPU_VERBOSE >= 2
			//PRINT_CLASS_FUNC_NAME_CONT<<"execution_stop_time: "<<execution_stop_time<<std::endl;
			//PRINT_CLASS_FUNC_NAME_CONT<<"sampling stop time: "<<sampling_thread_controller.get_stop_time()<<std::endl;
			//#endif

			//time_t diff_sec=sampling_thread_controller.get_stop_time().tv_sec-execution_stop_time.tv_sec;
			//long diff_nsec=sampling_thread_controller.get_stop_time().tv_nsec-execution_stop_time.tv_nsec;
			//long long diff_compensation_period=diff_sec*(long long)1000000000+diff_nsec;
			//#if METERPU_VERBOSE >= 2
			//PRINT_CLASS_FUNC_NAME_CONT<<"diff_sec("<<diff_sec<<")"<<std::endl;
			//PRINT_CLASS_FUNC_NAME_CONT<<"diff_nsec("<<diff_nsec<<")"<<std::endl;
			//PRINT_CLASS_FUNC_NAME_CONT<<"diff_compensation_period("<<diff_compensation_period<<")"<<std::endl;
			//#endif
			//if(diff_compensation_period<0)
			//PRINT_CLASS_FUNC_NAME_CONT<<"Warning: Sampling stopped earlier than execution ends."<<std::endl;
			//out1<<diff_sec<<","<<diff_nsec<<","<<static_power_value<<std::endl;

			//}

			class Sampling_Thread_Controller 
				: public Measurement_Controller
			{
				/*{{{*/
				public:
					Sampling_Thread_Controller():power(get_UNREALISTIC_POWER_VALUE()){}
				private:
					DECLARE_CLASS_NAME("Sampling_Thread_Controller");
					//lu/ A good encapsulation to define a constant
					static NVML_Energy<>::Power_Unit get_UNREALISTIC_POWER_VALUE(){ return (NVML_Energy<>::Power_Unit)100000000;}
					/*Static bool sampling;*/
					bool sampling;
					//! Device handle.
					/*!
					  initialized by device_id.
					  */
					nvmlDevice_t device;
					//! Power value for each sample.
					/*!
					  Here as a member varialbe, to save overhead for variable declaration each time.
					  */
					NVML_Energy<>::Power_Unit power;
					//! Time value for each sample.
					/*!
					  Here as a member varialbe, to save overhead for variable declaration each time.
					  */
					NVML_Energy<>::Time_Unit time;
					//! Power data database.
					/*!
					  Store time and power value pairs.
					  */
					NVML_Energy<>::Time_DB_Type time_db;
					NVML_Energy<>::Power_DB_Type power_db;
					static void *thread_program(void *arg)
					{
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("start");
#endif

						bool * const sampling_local=&((Sampling_Thread_Controller *)arg)->sampling;
						NVML_Energy<>::Time_Unit * const time_local=&((Sampling_Thread_Controller *)arg)->time;
						NVML_Energy<>::Power_Unit * const power_local=&((Sampling_Thread_Controller *)arg)->power;
						NVML_Energy<>::Time_DB_Type * const time_db_local=&((Sampling_Thread_Controller *)arg)->time_db;
						NVML_Energy<>::Power_DB_Type * const power_db_local=&((Sampling_Thread_Controller *)arg)->power_db;
						nvmlDevice_t const device_local=((Sampling_Thread_Controller *)arg)->device;

						nvmlReturn_t return_code; 
#if METERPU_VERBOSE >= 2
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling_local("<<*sampling_local<<")\n";
#endif

						while(*sampling_local)
						{

							//Sampling power
							return_code = nvmlDeviceGetPowerUsage( device_local, power_local );
							if ( return_code != NVML_SUCCESS) { 
								continue;
							}
							//Sampling time, do it after sampling power,
							//because sampling power is costly,
							//time stamp is taken long time ago
							//after a power sample is obtained,
							//may give the false feeling that sampling stops ealier
							//than the release of stop signal
							//clock_gettime(CLOCK_MONOTONIC, time_local);
							METERPU_TIME_MEASURE(time_local);

#if METERPU_VERBOSE >= 4
							PRINT_CLASS_FUNC_NAME_CONT<<"current power("<<*power_local<<")\n";
#endif

							//Add time and power pair to database
							//lu/ it is a deep copy, thus safe!
							power_db_local->push_back(*power_local);
							time_db_local->push_back(*time_local);
						}

#if METERPU_VERBOSE >= 2
						NVML_Energy<>::Time_DB_Type::iterator it=time_db_local->end();
						PRINT_CLASS_FUNC_NAME_CONT<<"Obtained stop signal, the last time stamp of sampling is: "
							<<*--it
							<<std::endl;
#endif

						pthread_exit((void*) arg);
					}
					pthread_t thread;
					pthread_attr_t attr;
					//void persist_DB() const{
					//#if METERPU_VERBOSE >= 1
					//PRINT_CLASS_FUNC_NAME("called");
					//#endif

					////write power data to a file as original form, keep precision
					////format "tvsec","tvnsec","power"
					////        1,0,1
					//std::ofstream out1("appgen_power.csv");
					//if(!out1){
					//std::cout<<"File creation error!"<<std::endl;
					//exit(1);
					//}
					//out1<<"\"tvsec\",\"tvnsec\",\"power\""<<std::endl;
					//for(unsigned int i=0;i<time_db.size();++i)
					//out1<<time_db[i].tv_sec<<","<<time_db[i].tv_nsec<<","<<power_db[i]<<std::endl;

					//}
				public:
					void start()
					{
						int rc;

						pthread_attr_init(&attr);
						pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

						//Set the thread runnable flag
						sampling=true;

#if METERPU_VERBOSE >= 2
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling("<<sampling<<")\n";
#endif

						//Release the thread
						rc = pthread_create(&thread, &attr, thread_program, (void *)this); 
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("thread released");
#endif
						if (rc) {
							printf("ERROR; return code from pthread_create() is %d\n", rc);
							exit(-1);
						}
					}
					void stop(){
						int rc;
						void *status;

						//Set stop signal
						sampling=false;

						//Wait for thread to join
						rc = pthread_join(thread, &status);
						if (rc) {
							printf("ERROR; return code from pthread_join() is %d\n", rc);
							exit(-1);
						}
						pthread_attr_destroy(&attr);

#if METERPU_VERBOSE >= 1

						NVML_Energy<>::Time_DB_Type::iterator it=time_db.end();
						CPU_Time_Measurement_Controller temp(*time_db.begin(),*--it);
						temp.calc();
						PRINT_CLASS_FUNC_NAME_CONT<<"time period of sampling is: "
							<<temp.get_value()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"number of sampling is: "
							<<power_db.size()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"The first sample of power is: "
							<<*power_db.begin()
							<<std::endl;
						PRINT_CLASS_FUNC_NAME_CONT<<"sampling rate is: "
							<<power_db.size()/temp.get_value()
							<<" #samples/sec."
							<<std::endl;
#endif


					}
					void calc(){
#if METERPU_VERBOSE >= 1
						PRINT_CLASS_FUNC_NAME("called");
#endif
						//persist_DB();

						//reset state for next measurement
						reset_state();

					}
					void show_meter_reading() const {PRINT_FUNC_NAME_CONT<<"Not applicable to call me..."<<std::endl;}
					void set_device(const nvmlDevice_t &device_handle){device=device_handle;}
					const NVML_Energy<>::Power_Unit &get_power() const {return power;}
					const NVML_Energy<>::Power_DB_Type &get_power_db() const {return power_db;}
					const NVML_Energy<>::Time_DB_Type &get_time_db() const {return time_db;}
					NVML_Energy<>::Time_Unit const &get_stop_time(){
						assert(time_db.size()>=1);
						//TODO: use back() method here
						NVML_Energy<>::Time_DB_Type::iterator it=time_db.end();
						return *--it;
					}
					NVML_Energy<>::Time_Unit const &get_start_time(){
						assert(time_db.size()>=1);
						//NVML_Energy<>::Time_DB_Type::iterator it=time_db.begin();
						//return *it;
						//TODO: use front() method here
						return *time_db.begin();
					}
					void reset_state(){time_db.clear();power_db.clear();}

					/*}}}*/
			};
			Sampling_Thread_Controller sampling_thread_controller;
			/*}}}*/
	};
#endif


#ifdef ENABLE_SYSTEM_ENERGY

	//Micro for manually loop unrolling
	/*{{{*/

	/**
	 *  \brief A macro for downcast a void pointer to 
	 *  a NVML Energy Meter.
	 *  
	 *  
	 */
#define POINTER ((Meter<NVML_Energy<first_device_id> >*)x[array_index])

#define NOTHING

	/**
	 *  \brief A macro to build variadic template
	 *  to recursively apply code snippets
	 *  
	 *  
	 */
#define LOOPER(name, return_type, base_code, general_code) \
	template <GPU_Device_Id_Type ...default_case>  \
	struct name { static return_type apply(void *x[]) { base_code  } }; \
	template <GPU_Device_Id_Type array_index,  \
	GPU_Device_Id_Type first_device_id,  \
	GPU_Device_Id_Type ...rest> \
	struct name <array_index,first_device_id, rest...> \
	{ \
		static return_type apply(void *x[]) \
		{ \
			general_code \
			name <array_index+1,rest...>::apply(x); \
		} \
	}; 
	/*}}}*/

	//!  System Energy Measurement Controller. 
	/*!
	  It initialize a Energy Meter of CPU and DRAM, together with a specified combination of GPU Energy Meters,
	  thus it can measure energy consumption of those hardware components together.
	*/
	template<GPU_Device_Id_Type ...gpu_device_ids>
		struct System_Energy_Measurement_Controller : public Measurement_Controller
	{
		/*{{{*/
		private:
			LOOPER(GPU_Meter_Constructor, void, NOTHING, x[array_index]=new Meter<NVML_Energy<first_device_id> >;)
			LOOPER(GPU_Meter_Destructor, void, NOTHING, delete POINTER;)
			LOOPER(GPU_Meter_Start, void, NOTHING, POINTER->start();)
			LOOPER(GPU_Meter_Stop, void, NOTHING, POINTER->stop();)
			LOOPER(GPU_Meter_Cal, void, NOTHING, POINTER->calc();)
			LOOPER(GPU_Meter_Get_Meter_Reading, NVML_Energy<>::ResultType, return 0;, return POINTER->get_value()+)

		private:
				Meter<PCM_Energy> cpu_meter;
				enum{N=sizeof...(gpu_device_ids)};
				void* gpu_meters[N];
				NVML_Energy<>::ResultType meter_reading;
		public:
				System_Energy_Measurement_Controller(){
					GPU_Meter_Constructor<0,gpu_device_ids...>::apply(gpu_meters);
				}
				~System_Energy_Measurement_Controller(){
					GPU_Meter_Destructor<0,gpu_device_ids...>::apply(gpu_meters);
				}
				inline void init(){}
				inline void start(){
					GPU_Meter_Start<0,gpu_device_ids...>::apply(gpu_meters);
					cpu_meter.start();
				}
				inline void stop(){
					cpu_meter.stop();
					GPU_Meter_Stop<0,gpu_device_ids...>::apply(gpu_meters);
				}
				inline void calc(){
					meter_reading=0;
					cpu_meter.calc();
					meter_reading+=cpu_meter.get_value();
					GPU_Meter_Cal<0,gpu_device_ids...>::apply(gpu_meters);
					meter_reading=cpu_meter.get_value()
						+ GPU_Meter_Get_Meter_Reading<0,gpu_device_ids...>::apply(gpu_meters)
						;
				}
		/**
		 *  \brief Get calculated metric value, require calc() to be
		 *  called already.
		 *  
		 *  
		 */
				inline System_Energy<>::ResultType const &get_value() const{ return meter_reading;}
				inline void show_meter_reading() const {std::cout<<"[System Energy Meter] Energy consumed is: "<<get_value()<<" milli Joules."<<std::endl;}
				/*}}}*/
	};
#endif


	/** @} */ // end of group3
	/*}}}*/


}
/*}}}*/


#endif
