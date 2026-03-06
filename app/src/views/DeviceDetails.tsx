import { useState } from 'react';
import { ArrowLeft, DoorOpen, Phone, User, Trash2, UserMinus, CheckCircle2 } from 'lucide-react';
import { View } from '../App';

export default function DeviceDetails({ onNavigate, deviceId }: { onNavigate: (view: View) => void, deviceId: string | null }) {
  const [showRevoke, setShowRevoke] = useState(false);
  const [userToRevoke, setUserToRevoke] = useState<string | null>(null);

  const handleRevoke = (name: string) => {
    setUserToRevoke(name);
    setShowRevoke(true);
  };

  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden relative">
      <header className="pt-6 px-4 pb-4 border-b border-slate-800 bg-background-dark sticky top-0 z-10">
        <div className="flex items-center justify-between">
          <button onClick={() => onNavigate('devices')} className="p-2 -ml-2 text-slate-100">
            <ArrowLeft size={24} />
          </button>
          <h1 className="text-xl font-bold tracking-tight">Device Details</h1>
          <div className="w-10"></div>
        </div>
      </header>

      <main className="flex-1 overflow-y-auto hide-scrollbar px-4 py-6 space-y-8 pb-40">
        <div className="flex items-center gap-4">
          <div className="size-16 rounded-xl bg-primary/10 flex items-center justify-center shrink-0">
            <DoorOpen size={36} className="text-primary" />
          </div>
          <div>
            <h2 className="text-2xl font-bold text-slate-100">Office Front Door</h2>
            <p className="text-sm font-medium text-slate-400 mt-0.5">ID: {deviceId || '882-991-002'}</p>
          </div>
        </div>

        <section>
          <h3 className="text-lg font-bold mb-3 text-slate-100">Device Information</h3>
          <div className="bg-slate-900/40 border border-slate-800 rounded-xl p-4 shadow-sm">
            <div className="flex justify-between items-center">
              <span className="text-slate-400 font-medium text-sm">Serial Number</span>
              <span className="font-semibold text-slate-100">SN-99X2-114A</span>
            </div>
          </div>
        </section>

        <section>
          <h3 className="text-lg font-bold mb-3 text-slate-100">Share Access</h3>
          <div className="bg-slate-900/40 border border-slate-800 rounded-xl p-5 shadow-sm space-y-4">
            <p className="text-sm text-slate-400 leading-relaxed">
              Enter a phone number to grant unlock permissions to a new user. They will receive an SMS invitation.
            </p>
            <div>
              <label className="block text-xs font-bold text-slate-400 uppercase tracking-wider mb-2">Phone Number</label>
              <div className="flex gap-2">
                <div className="relative flex-1">
                  <div className="absolute inset-y-0 left-0 pl-3 flex items-center pointer-events-none text-slate-400">
                    <Phone size={18} />
                  </div>
                  <input 
                    type="tel" 
                    placeholder="+1 (555) 000-0000" 
                    className="block w-full pl-10 pr-4 py-3 bg-slate-800/50 border-none rounded-xl focus:ring-2 focus:ring-primary text-sm placeholder:text-slate-400 text-slate-100 outline-none"
                  />
                </div>
                <button className="px-5 py-3 bg-slate-700 text-slate-200 rounded-xl text-sm font-bold transition-colors hover:bg-slate-600">
                  Invite
                </button>
              </div>
            </div>
          </div>
        </section>

        <section>
          <h3 className="text-lg font-bold mb-3 text-slate-100">Authorized Users</h3>
          <div className="bg-slate-900/40 border border-slate-800 rounded-xl overflow-hidden shadow-sm">
            <div className="divide-y divide-slate-800">
              <div className="flex items-center justify-between p-4">
                <div className="flex items-center gap-3">
                  <div className="size-10 rounded-full bg-slate-800 flex items-center justify-center">
                    <User size={20} className="text-slate-500" />
                  </div>
                  <div className="flex flex-col">
                    <span className="text-sm font-semibold text-slate-100">Owner</span>
                    <span className="text-xs text-slate-400">+1 (555) 123-4567</span>
                  </div>
                </div>
                <span className="px-2 py-1 bg-slate-800 text-[10px] font-bold text-slate-500 uppercase rounded tracking-wider">Default</span>
              </div>
              
              <div className="flex items-center justify-between p-4">
                <div className="flex items-center gap-3">
                  <div className="size-10 rounded-full bg-primary/10 flex items-center justify-center">
                    <User size={20} className="text-primary" />
                  </div>
                  <div className="flex flex-col">
                    <span className="text-sm font-semibold text-slate-100">John Smith</span>
                    <span className="text-xs text-slate-400">+1 (555) 987-6543</span>
                  </div>
                </div>
                <button onClick={() => handleRevoke('John Smith')} className="p-2 text-slate-400 hover:text-red-500 transition-colors">
                  <Trash2 size={22} />
                </button>
              </div>

              <div className="flex items-center justify-between p-4">
                <div className="flex items-center gap-3">
                  <div className="size-10 rounded-full bg-primary/10 flex items-center justify-center">
                    <User size={20} className="text-primary" />
                  </div>
                  <div className="flex flex-col">
                    <span className="text-sm font-semibold text-slate-100">Sarah Wilson</span>
                    <span className="text-xs text-slate-400">+1 (555) 444-2211</span>
                  </div>
                </div>
                <button onClick={() => handleRevoke('Sarah Wilson')} className="p-2 text-slate-400 hover:text-red-500 transition-colors">
                  <Trash2 size={22} />
                </button>
              </div>
            </div>
          </div>
        </section>
      </main>

      <div className="absolute bottom-0 left-0 right-0 px-4 pb-8 pt-4 bg-background-dark/95 backdrop-blur-md border-t border-slate-800 space-y-3 z-20">
        <button className="w-full py-3.5 bg-primary text-white rounded-xl font-bold text-base shadow-sm shadow-primary/30 flex items-center justify-center gap-2 active:scale-[0.98] transition-transform">
          <CheckCircle2 size={20} />
          Save Changes
        </button>
        <button className="w-full py-3.5 bg-red-500/10 text-red-500 rounded-xl font-bold text-base border border-red-500/20 flex items-center justify-center gap-2 active:scale-[0.98] transition-transform">
          <Trash2 size={20} />
          Remove Device
        </button>
      </div>

      {/* Revoke Access Dialog */}
      {showRevoke && (
        <div className="fixed inset-0 bg-[#221011]/80 backdrop-blur-sm z-40 flex items-center justify-center p-6 animate-in fade-in duration-200">
          <div className="w-full max-w-sm bg-slate-900 rounded-xl overflow-hidden shadow-2xl border border-slate-800 flex flex-col items-center text-center p-6 gap-6">
            <div className="w-16 h-16 rounded-full bg-[#ec131e]/10 flex items-center justify-center">
              <UserMinus size={32} className="text-[#ec131e]" />
            </div>
            <div className="space-y-2">
              <h3 className="text-slate-100 text-2xl font-bold leading-tight">Revoke Access?</h3>
              <p className="text-slate-400 text-sm leading-relaxed px-2">
                Are you sure you want to remove access for <span className="font-semibold text-slate-100">{userToRevoke}</span>? This action cannot be undone and they will immediately lose the ability to unlock this device.
              </p>
            </div>
            <div className="flex flex-col w-full gap-3">
              <button 
                onClick={() => setShowRevoke(false)}
                className="w-full h-12 bg-[#ec131e] hover:bg-[#ec131e]/90 text-white rounded-lg font-bold text-base transition-colors flex items-center justify-center"
              >
                Revoke Access
              </button>
              <button 
                onClick={() => setShowRevoke(false)}
                className="w-full h-12 bg-slate-800 hover:bg-slate-700 text-slate-100 rounded-lg font-semibold text-base transition-colors flex items-center justify-center"
              >
                Cancel
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
