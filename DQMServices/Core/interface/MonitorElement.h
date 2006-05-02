#ifndef MonitorElement_h
#define MonitorElement_h


#include <sys/time.h>

#include <string>
#include <map>

#include "Utilities/General/interface/MutexUtils.h"

#include "DQMServices/Core/interface/DQMDefinitions.h"
#include "DQMServices/Core/interface/QReport.h"

/** The base class for all MonitorElements (ME)
 */
class MonitorElement
{

 public:
  
  MonitorElement();
  MonitorElement(const char*);
  virtual ~MonitorElement();

  // true if ME was updated in last monitoring cycle
  bool wasUpdated() const;
  // specify whether ME should be reset at end of monitoring cycle (default: false);
  // (typically called by Sources that control the original ME)
  void setResetMe(bool flag);
  // get name of ME
  virtual std::string getName() const  = 0;
  // "Fill" ME methods:
  // can be used with 1D histograms or scalars
  virtual void Fill(float)=0;
  // can be used with 2D (x,y) or 1D (x, w) histograms
  virtual void Fill(float, float)=0;
  // can be used with 3D (x, y, z) or 2D (x, y, w) histograms
  virtual void Fill(float, float, float)=0;
  // can be used with 3D (x, y, z, w) histograms
  virtual void Fill(float, float, float, float)=0;
  // returns value of ME in string format (eg. "f = 3.14151926" for float numbers);
  // relevant only for scalar or string MEs
  virtual std::string valueString() const=0;
  // return tagged value of ME in string format 
  // (eg. <name>f=3.14151926</name> for float numbers);
  // relevant only for sending scalar or string MEs over TSocket
  std::string tagString() const
  {return "<" + getName() + ">" + valueString() + "</" + getName() + ">";}
  // true if ME is a folder
  bool isFolder(void) const;
  // opposite of isFolder method
  bool isNotFolder(void) const;

  // true if at least of one of the quality tests returned an error
  // (will override for folders)
  virtual bool hasError(void) const {return !qerrors_.empty();}
  // true if at least of one of the quality tests returned a warning
  // (will override for folders)
  virtual bool hasWarning(void) const {return !qwarnings_.empty();}
  // true if at least of one of the tests returned some other (non-ok) status
  // (will override for folders)
  virtual bool hasOtherReport(void) const {return !qothers_.empty();}

  // get QReport corresponding to <qtname> (null pointer if QReport does not exist)
  const QReport * getQReport(std::string qtname) const;

  // get map of QReports
  dqm::qtests::QR_map getQReports(void) const {return qreports_;}
  // get warnings from last set of quality tests
  std::vector<QReport *> getQWarnings(void) const {return qwarnings_;}
  // get errors from last set of quality tests
  std::vector<QReport *> getQErrors(void) const {return qerrors_;}
  // get "other" (i.e. non-error, non-warning, non-"ok") QReports 
  // from last set of quality tests
  std::vector<QReport *> getQOthers(void) const {return qothers_;}

  // run all quality tests
  void runQTests(void);

  /*** getter methods (wrapper around ROOT methods) ****/
  // 
  // get mean value of histogram along x, y or z axis (axis=1, 2, 3 respectively)
  virtual float getMean(int axis = 1) const = 0;
  // get mean value uncertainty of histogram along x, y or z axis 
  // (axis=1, 2, 3 respectively)
  virtual float getMeanError(int axis = 1) const = 0;
  // get RMS of histogram along x, y or z axis (axis=1, 2, 3 respectively)
  virtual float getRMS(int axis = 1) const = 0;
  // get RMS uncertainty of histogram along x, y or z axis (axis=1,2,3 respectively)
  virtual float getRMSError(int axis = 1) const = 0;
  // get content of bin (1-D)
  virtual float getBinContent(int binx) const = 0;
  // get content of bin (2-D)
  virtual float getBinContent(int binx, int biny) const = 0;
  // get content of bin (3-D)
  virtual float getBinContent(int binx, int biny, int binz) const = 0;
  // get uncertainty on content of bin (1-D) - See TH1::GetBinError for details
  virtual float getBinError(int binx) const = 0;
  // get uncertainty on content of bin (2-D) - See TH1::GetBinError for details
  virtual float getBinError(int binx, int biny) const = 0;
  // get uncertainty on content of bin (3-D) - See TH1::GetBinError for details
  virtual float getBinError(int binx, int biny, int binz) const = 0;
  // get # of entries
  virtual float getEntries(void) const = 0;
  // get # of bin entries (for profiles)
  virtual float getBinEntries(int bin) const = 0;
  // get min Y value (for profiles)
  virtual float getYmin(void) const = 0;
  // get max Y value (for profiles)
  virtual float getYmax(void) const = 0;

  /*** setter methods (wrapper around ROOT methods) ****/
  // 
  // set content of bin (1-D)
  virtual void setBinContent(int binx, float content) = 0;
  // set content of bin (2-D)
  virtual void setBinContent(int binx, int biny, float content) = 0;
  // set content of bin (3-D)
  virtual void setBinContent(int binx, int biny, int binz, float content) = 0;
  // set uncertainty on content of bin (1-D)
  virtual void setBinError(int binx, float error) = 0;
  // set uncertainty on content of bin (2-D)
  virtual void setBinError(int binx, int biny, float error) = 0;
  // set uncertainty on content of bin (3-D)
  virtual void setBinError(int binx, int biny, int binz, float error) = 0;
  // set # of entries
  virtual void setEntries(float nentries) = 0;
  // set bin label for x, y or z axis (axis=1, 2, 3 respectively)
  virtual void setBinLabel(int bin, std::string label, int axis = 1) = 0;
  // set x-, y- or z-axis range (axis=1, 2, 3 respectively)
  virtual void setAxisRange(float xmin, float xmax, int axis = 1) = 0;

  LockMutex::Mutex mutex;

 private:
  
 protected:

  void update();
  // reset ME (ie. contents, errors, etc)
  virtual void Reset()=0;

  // ------------ Operations for MEs that are normally never reset ---------

  // reset contents (does not erase contents permanently)
  // (makes copy of current contents; will be subtracted from future contents)
  virtual void softReset(void){}

  // reverts action of softReset
  virtual void disableSoftReset(void){}

  // --- Operations on MEs that are normally reset at end of monitoring cycle ---

  // if true, will accumulate ME contents (over many periods)
  // until method is called with flag = false again
  void setAccumulate(bool flag);

  // whether soft-reset is enabled
  virtual bool isSoftResetEnabled(void) const = 0; // default: false
  // whether ME contents should be accumulated over multiple monitoring periods
  bool accumulate_on; // default: false

  // true if ME should be reset at end of monitoring cycle
  bool resetMe(void) const;
  // reset "was updated" flag
  void resetUpdate();
  // add quality report (to be called by DaqMonitorROOTBackEnd)
  void addQReport(QReport * qr);
  // true if QReport with name <qtname> already exists
  bool qreportExists(std::string qtname) const;

  class manage{
  public:
    manage() : variedSince(true), folder_flag(false),
      resetMe(false){}
    // has content changed?
    bool variedSince; 
    // is this a folder? (if not, it's a monitoring object)
    bool folder_flag;
    // should contents be reset at end of monitoring cycle?
    bool resetMe;
    // creation time
    timeval time_stamp;
  };

  manage man;

  dqm::qtests::QR_map qreports_;

  // warnings from last set of quality tests
  std::vector<QReport *> qwarnings_;
  // errors from last set of quality tests
  std::vector<QReport *> qerrors_;
  // "other" (i.e. non-error, non-warning, non-"ok") QReports
  std::vector<QReport *> qothers_;

  friend class DaqMonitorBEInterface;
  friend class CollateMET;

};

#endif









