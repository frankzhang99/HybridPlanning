/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class PlanDB {
  private transient long swigCPtr;
  protected transient boolean swigCMemOwn;

  protected PlanDB(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(PlanDB obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        throw new UnsupportedOperationException("C++ destructor does not have public access");
      }
      swigCPtr = 0;
    }
  }

  public static PlanDB get_instance() {
    long cPtr = DBJNI.PlanDB_get_instance();
    return (cPtr == 0) ? null : new PlanDB(cPtr, false);
  }

  public boolean populate_db(String dir) {
    return DBJNI.PlanDB_populate_db(swigCPtr, this, dir);
  }

  public void destroy_db() {
    DBJNI.PlanDB_destroy_db(swigCPtr, this);
  }

  public void clean_db() {
    DBJNI.PlanDB_clean_db(swigCPtr, this);
  }

  public boolean get_plan(SWIGTYPE_p_HPModel pModel, SWIGTYPE_p_std__vectorT_std__string_t plan) {
    return DBJNI.PlanDB_get_plan(swigCPtr, this, SWIGTYPE_p_HPModel.getCPtr(pModel), SWIGTYPE_p_std__vectorT_std__string_t.getCPtr(plan));
  }

  public SWIGTYPE_p_std__vectorT_std__string_t get_actions(SWIGTYPE_p_State state) {
    return new SWIGTYPE_p_std__vectorT_std__string_t(DBJNI.PlanDB_get_actions(swigCPtr, this, SWIGTYPE_p_State.getCPtr(state)), true);
  }

}
