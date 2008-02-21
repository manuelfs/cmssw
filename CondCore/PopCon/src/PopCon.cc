#include "CondCore/PopCon/interface/PopCon.h"
#include "CondCore/PopCon/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include<iostream>

namespace popcon {

  PopCon::PopCon(const edm::ParameterSet& pset):
    m_record(pset.getParameter<std::string> ("record")),
    m_payload_name(pset.getUntrackedParameter<std::string> ("name","")),
    m_since(pset.getParameter<bool> ("SinceAppendMode")),
    m_LoggingOn(pset.getUntrackedParameter< bool > ("loggingOn",true))
    {
    //TODO set the policy (cfg or global configuration?)
    //Policy if corrupted data found

      edm::LogInfo ("PopCon") << "This is PopCon (Populator of Condition) V2.00\n"
        << "It is still in active developement\n"
			  << "We apologise for the verbosity of the output, for the criptic messages, for the limited functionalities and the poor documentation\n"<<
    "Please report any problem and feature request through the savannah portal under the category conditions\n" ; 

    }
  
  PopCon::~PopCon(){}
 

  void PopCon::initialize() {	
    edm::LogInfo ("PopCon")<<"payload name "<<m_payload_name<<std::endl;
    if(!m_dbService.isAvailable() ) throw Exception("DBService not available");
    
    m_tag = m_dbService->tag(m_record);
    if (!m_dbService->isNewTagRequest(m_record) ) {
      m_dbService->tagInfo(m_record,m_tagInfo);
      m_dbService->queryLog().LookupLastEntryByTag(m_tag, m_logDBEntry);
      
      edm::LogInfo ("PopCon") << "TAG, last since/till, size " << m_tag 
		<< ", " <<  m_tagInfo.lastInterval.first
		<< "/" << m_tagInfo.lastInterval.second
			      << ", " << m_tagInfo.size << "\n" <<
      "Last writer, size " <<  m_logDBEntry.provenance 
		<< ", " << m_logDBEntry.payloadIdx+1 << std::endl;
    }
  }


  void PopCon::finalize() {
  }

}
