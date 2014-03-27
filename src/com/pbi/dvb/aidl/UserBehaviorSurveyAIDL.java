/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: /home/klin/workspace/DVB_AMLOGIC_SX_SVN/src/com/pbi/dvb/aidl/UserBehaviorSurveyAIDL.aidl
 */
package com.pbi.dvb.aidl;
public interface UserBehaviorSurveyAIDL extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.pbi.dvb.aidl.UserBehaviorSurveyAIDL
{
private static final java.lang.String DESCRIPTOR = "com.pbi.dvb.aidl.UserBehaviorSurveyAIDL";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.pbi.dvb.aidl.UserBehaviorSurveyAIDL interface,
 * generating a proxy if needed.
 */
public static com.pbi.dvb.aidl.UserBehaviorSurveyAIDL asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.pbi.dvb.aidl.UserBehaviorSurveyAIDL))) {
return ((com.pbi.dvb.aidl.UserBehaviorSurveyAIDL)iin);
}
return new com.pbi.dvb.aidl.UserBehaviorSurveyAIDL.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_pushBehavior:
{
data.enforceInterface(DESCRIPTOR);
java.lang.String _arg0;
_arg0 = data.readString();
java.lang.String _arg1;
_arg1 = data.readString();
int _arg2;
_arg2 = data.readInt();
boolean _arg3;
_arg3 = (0!=data.readInt());
java.lang.String[] _arg4;
_arg4 = data.createStringArray();
this.pushBehavior(_arg0, _arg1, _arg2, _arg3, _arg4);
reply.writeNoException();
reply.writeStringArray(_arg4);
return true;
}
case TRANSACTION_pushBehaviorNew:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
boolean _arg1;
_arg1 = (0!=data.readInt());
java.lang.String[] _arg2;
_arg2 = data.createStringArray();
this.pushBehaviorNew(_arg0, _arg1, _arg2);
reply.writeNoException();
reply.writeStringArray(_arg2);
return true;
}
case TRANSACTION_generateActionOne:
{
data.enforceInterface(DESCRIPTOR);
com.pbi.dvb.domain.PiwikTrackerBehaviorBean _result = this.generateActionOne();
reply.writeNoException();
if ((_result!=null)) {
reply.writeInt(1);
_result.writeToParcel(reply, android.os.Parcelable.PARCELABLE_WRITE_RETURN_VALUE);
}
else {
reply.writeInt(0);
}
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.pbi.dvb.aidl.UserBehaviorSurveyAIDL
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public void pushBehavior(java.lang.String project, java.lang.String action, int type, boolean unInterruptableFlag, java.lang.String[] tags) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeString(project);
_data.writeString(action);
_data.writeInt(type);
_data.writeInt(((unInterruptableFlag)?(1):(0)));
_data.writeStringArray(tags);
mRemote.transact(Stub.TRANSACTION_pushBehavior, _data, _reply, 0);
_reply.readException();
_reply.readStringArray(tags);
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public void pushBehaviorNew(int type, boolean unInterruptableFlag, java.lang.String[] tags) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(type);
_data.writeInt(((unInterruptableFlag)?(1):(0)));
_data.writeStringArray(tags);
mRemote.transact(Stub.TRANSACTION_pushBehaviorNew, _data, _reply, 0);
_reply.readException();
_reply.readStringArray(tags);
}
finally {
_reply.recycle();
_data.recycle();
}
}
@Override public com.pbi.dvb.domain.PiwikTrackerBehaviorBean generateActionOne() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
com.pbi.dvb.domain.PiwikTrackerBehaviorBean _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_generateActionOne, _data, _reply, 0);
_reply.readException();
if ((0!=_reply.readInt())) {
_result = com.pbi.dvb.domain.PiwikTrackerBehaviorBean.CREATOR.createFromParcel(_reply);
}
else {
_result = null;
}
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_pushBehavior = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_pushBehaviorNew = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_generateActionOne = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
}
public void pushBehavior(java.lang.String project, java.lang.String action, int type, boolean unInterruptableFlag, java.lang.String[] tags) throws android.os.RemoteException;
public void pushBehaviorNew(int type, boolean unInterruptableFlag, java.lang.String[] tags) throws android.os.RemoteException;
public com.pbi.dvb.domain.PiwikTrackerBehaviorBean generateActionOne() throws android.os.RemoteException;
}
