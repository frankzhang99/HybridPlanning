/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.0.0
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class AP {
  public static SWIGTYPE_p_std__vectorT_std__string_t getActions(String adversaryPath, String labelsPath, SWIGTYPE_p_std__setT_int_t states) {
    return new SWIGTYPE_p_std__vectorT_std__string_t(APJNI.getActions(adversaryPath, labelsPath, SWIGTYPE_p_std__setT_int_t.getCPtr(states)), true);
  }

  public static SWIGTYPE_p_std__setT_int_t getNowStates(String statesPath) {
    return new SWIGTYPE_p_std__setT_int_t(APJNI.getNowStates(statesPath), true);
  }

}