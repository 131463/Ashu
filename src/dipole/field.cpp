//------------------------------------------------------------------------------
//
//   This file is part of the VAMPIRE open source package under the
//   Free BSD licence (see licence file for details).
//
//   (c) Andrea Meo and Richard F L Evans 2016. All rights reserved.
//
//------------------------------------------------------------------------------
//

// C++ standard library headers
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>


// Vampire headers
#include "dipole.hpp"
#include "vmpi.hpp"
#include "cells.hpp"
#include "vio.hpp"
#include "errors.hpp"

// dipole module headers
#include "internal.hpp"

#include "sim.hpp"

namespace dipole{

   //-----------------------------------------------------------------------------
   // Function for updating local temperature fields
   //-----------------------------------------------------------------------------


	void calculate_field(){
      //dipole::internal::enabled=true;
      /*if(dipole::activated){
			terminaltextcolor(RED);
         std::cout << "Calculate dipolar field activated." << std::endl;
			terminaltextcolor(WHITE);
		}*/

		if(err::check==true){
			terminaltextcolor(RED);
			std::cerr << "dipole::field has been called " << vmpi::my_rank << std::endl;
			terminaltextcolor(WHITE);
		}

		// prevent double calculation for split integration (MPI)
		if(dipole::internal::update_time!=sim::time){

			// Check if update required
		   if(sim::time%dipole::update_rate==0){

			   //if updated record last time at update
			   dipole::internal::update_time=sim::time;

			   // update cell magnetisations
			   cells::mag();

			   // recalculate demag fields
            dipole::internal::update_field();

			   // For MPI version, only add local atoms
			   #ifdef MPICF
				   const int num_local_atoms = vmpi::num_core_atoms+vmpi::num_bdry_atoms;
			   #else
				   const int num_local_atoms = dipole::internal::num_atoms;
			   #endif

			   // Update Atomistic Dipolar Field Array
			   for(int atom=0;atom<num_local_atoms;atom++){
				   const int cell = dipole::internal::atom_cell_array[atom];
   	         if(dipole::internal::cells_num_atoms_in_cell[cell]>0){
				      // Copy field from macrocell to atomistic spin
				      dipole::atom_dipolar_field_array_x[atom]=dipole::cells_field_array_x[cell];
				      dipole::atom_dipolar_field_array_y[atom]=dipole::cells_field_array_y[cell];
				      dipole::atom_dipolar_field_array_z[atom]=dipole::cells_field_array_z[cell];
   	         }
			   }

		   } // End of check for update rate
		} // end of check for update time
   }

} // end of dipole namespace