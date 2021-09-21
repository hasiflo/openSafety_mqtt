pysodbuilder
Version 1.5.0
============

This program is part of the openSAFETY source distribution.
It allows the customisation and the generation of the files
EPLScfg.h, sod.h and sod.c needed for openSAFETY applications.

Installation: python setup.py install

pysodbuilder depends on the following third party packages:
- cogapp 2.4: see https://pypi.python.org/pypi/cogapp/2.4
- PyXB 1.2.4: see https://pypi.python.org/pypi/PyXB/

Current limitations and kown issues:
====================================
- If multiple Channels of Type "Boolean" share the same object index and
  subindex, the Index attribute of the Transport element is not observed
  (accessing higher bytes), so only the first (lowest) byte is accessable.
- Repetition Elements are not recognized by the tool, so no appropriate code
 will be generated.
- No code will be generated for Conditions Elements in Setting Elements.
- Parsing settings out of the parameter Stream (SOD 0x101A/0x00) into the
  setting structures: generation of such code currently not implemented.
- No float types supported
- ManufacturerSpecificObjectDictionary Element not supported
- Only unsigned Range definitions supported