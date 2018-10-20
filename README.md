------------------------------------------------
CAxLib - A Process Planning Framework for Additive Manufacturing Applications
First release: Dec 12, 2017

Consiglio Nazionale delle Ricerche                                        
Istituto di Matematica Applicata e Tecnologie Informatiche                
Sezione di Genova                                                         
IMATI-GE / CNR                                                            

Authors:
- M. Attene, M. Livesu, D. Cabiddu

------------------------------------------------

CAxLib is a C++ toolkit which includes:
1) A library with C++ API to develop Process Planning software for Additive Manufacturing
2) A set of pre-implemented tools for the traditional operations along the Process Planning pipeline

Package contents:
CAxLib/
 Contains the library source code. It is a header library that must be simply included in downstream tools.

Tools/
 Contains source code for Process Planning tools that may depend either on CAxLib or on
 other libraries. Specifically, the following subfolders are included:
 CAxLib_Tools/ -> Contains tools that depend on CAxLib
 ImatiSTL_Tools -> Contains tools that depend on ImatiSTL (see below)
 CGAL_Tools -> Contains tools that depend on CGAL (see below)

Samples/
 Contains a sample model to be used for experimentation.

README.md
 This readme file.


External dependencies
 ImatiSTL_Tools can be compiled only after having downloaded and compiled ImatiSTL.
 Version 4.2-2 or higher is required.
 ImatiSTL can be downloaded at https://sourceforge.net/projects/imatistl/

 CGAL_Tools can be compiled and executed only after having downloaded and installed CGAL.
 Version 4.7 or higher required.
 CGAL can be downloaded at https://www.cgal.org/download.html

 CAxLib_Tools can be compiled and executed only after having downloaded and installed
 Zlib Version 1.2.11 (https://zlib.net), TinyXML2 (http://www.grinninglizard.com/tinyxml2/)
 and LibZIP Version 1.4.0 (https://libzip.org). Additionally, the mesh2vtu tool
 depends from VTK (https://www.vtk.org), and the tetrahedralize tool depends on Tetgen
 (http://wias-berlin.de/software/tetgen/). Also, the step_converters tool depends on the EXPRESS Data Manager (EDM). If you want 
 an evaluation or commercial license, please contact Jotne EPM Technology (web-address https://jira.jotne.com/servicedesk/customer
 /portal/2, e-mail helpdesk@jotne.com)

---------------------
Copyright and License
---------------------

CAxLib is

Copyright(C) 2017: IMATI-GE / CNR                                         

All rights reserved.                                                      

This program is dual-licensed as follows:

(1) You may use CAxLib as free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 3 of the License, or     
(at your option) any later version.                                      
In this case the program is distributed in the hope that it will be      
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of   
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
GNU General Public License (http://www.gnu.org/licenses/gpl.txt)         
for more details.                                                        

(2) You may use CAxLib for commercial purposes. In this case a
proper agreement must be reached with the Authors and with IMATI-GE/CNR  
based on a proper licensing contract.

----------------
Acknowledgements
----------------
This project has received funding from the European Unions Horizon 2020 research and innovation programme under grant agreement No 6 8044 8 (CAxMan - http://www.caxman.eu).
