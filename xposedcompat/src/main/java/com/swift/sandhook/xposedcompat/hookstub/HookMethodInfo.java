package com.swift.sandhook.xposedcompat.hookstub;

import com.swift.sandhook.utils.ParamWrapper;

public class HookMethodInfo {

    public Class[] parType;
    public Class retType;


    public Object getArg(int index, long address) {
        return ParamWrapper.addressToObject(parType[index], address);
    }

    public boolean isVoid() {
        return Void.TYPE.equals(retType);
    }

}
