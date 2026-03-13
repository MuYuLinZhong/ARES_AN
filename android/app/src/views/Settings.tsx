import { useState, useCallback } from 'react';
import { ArrowLeft, Smartphone, History, Wifi, Vibrate, Nfc, Info, LogOut, ChevronRight } from 'lucide-react';
import { View } from '../App';

declare global {
  interface Window {
    AndroidBridge?: {
      onToggleVibration(json: string): void;
      onNfcSensitivityChanged(json: string): void;
      onLogout(): void;
    };
  }
}

const SENSITIVITY_OPTIONS = ['Low', 'Medium', 'High'] as const;
type Sensitivity = typeof SENSITIVITY_OPTIONS[number];

export default function Settings({ onNavigate }: { onNavigate: (view: View) => void }) {
  const [vibrationEnabled, setVibrationEnabled] = useState(true);
  const [nfcSensitivity, setNfcSensitivity] = useState<Sensitivity>('Medium');

  const handleToggleVibration = useCallback(() => {
    const next = !vibrationEnabled;
    setVibrationEnabled(next);
    window.AndroidBridge?.onToggleVibration(JSON.stringify({ enabled: next }));
  }, [vibrationEnabled]);

  const handleCycleSensitivity = useCallback(() => {
    const idx = SENSITIVITY_OPTIONS.indexOf(nfcSensitivity);
    const next = SENSITIVITY_OPTIONS[(idx + 1) % SENSITIVITY_OPTIONS.length];
    setNfcSensitivity(next);
    window.AndroidBridge?.onNfcSensitivityChanged(JSON.stringify({ level: next }));
  }, [nfcSensitivity]);

  const handleLogout = useCallback(() => {
    window.AndroidBridge?.onLogout();
    onNavigate('login');
  }, [onNavigate]);

  return (
    <div className="flex-1 flex flex-col h-screen overflow-y-auto hide-scrollbar bg-[#15202b]">
      <header className="flex items-center p-4 pb-2 justify-between sticky top-0 bg-[#15202b]/90 backdrop-blur-sm z-10">
        <button onClick={() => onNavigate('home')} className="flex size-12 items-center justify-center text-slate-300 hover:text-primary transition-colors">
          <ArrowLeft size={24} />
        </button>
        <h2 className="text-lg font-bold flex-1 text-center pr-12">Settings</h2>
      </header>

      <div className="flex flex-col items-center pt-4 pb-8">
        <div className="w-24 h-24 rounded-full bg-orange-200 p-1 mb-4">
          <img src="https://picsum.photos/seed/alex/150/150" alt="User" className="w-full h-full rounded-full object-cover" />
        </div>
        <h1 className="text-2xl font-bold text-white mb-1">ARES User</h1>
        <p className="text-sm text-slate-400">Phase 1 • Local Mode</p>
      </div>

      <div className="px-4 space-y-6 pb-12">
        <section>
          <h3 className="text-xs font-bold text-primary uppercase tracking-wider mb-3 ml-2">Profile Management</h3>
          <div className="bg-[#1e2a38] rounded-xl overflow-hidden border border-slate-800">
            <div className="flex items-center justify-between p-4 border-b border-slate-800">
              <div className="flex items-center gap-3">
                <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                  <Smartphone size={20} className="text-primary" />
                </div>
                <span className="text-sm font-medium text-slate-200">Phone Number</span>
              </div>
              <div className="flex items-center gap-2 text-slate-400">
                <span className="text-sm text-slate-500">Phase 2</span>
              </div>
            </div>

            <div
              onClick={() => onNavigate('update-password')}
              className="flex items-center justify-between p-4 cursor-pointer hover:bg-slate-800/50 transition-colors"
            >
              <div className="flex items-center gap-3">
                <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                  <History size={20} className="text-primary" />
                </div>
                <span className="text-sm font-medium text-slate-200">Change Password</span>
              </div>
              <ChevronRight size={16} className="text-slate-400" />
            </div>
          </div>
        </section>

        <section>
          <h3 className="text-xs font-bold text-primary uppercase tracking-wider mb-3 ml-2">Working Mode</h3>
          <div className="bg-[#1e2a38] rounded-xl overflow-hidden border border-slate-800 p-4 flex items-center justify-between">
            <div className="flex items-center gap-3">
              <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                <Wifi size={20} className="text-slate-500" />
              </div>
              <div className="flex flex-col">
                <span className="text-sm font-medium text-slate-400">Online Mode</span>
                <span className="text-xs text-slate-600">Phase 2</span>
              </div>
            </div>
            <div className="w-12 h-6 bg-slate-700 rounded-full relative cursor-not-allowed opacity-50">
              <div className="absolute left-1 top-1 size-4 bg-white rounded-full" />
            </div>
          </div>
        </section>

        <section>
          <h3 className="text-xs font-bold text-primary uppercase tracking-wider mb-3 ml-2">System Settings</h3>
          <div className="bg-[#1e2a38] rounded-xl overflow-hidden border border-slate-800">
            <div
              onClick={handleToggleVibration}
              className="flex items-center justify-between p-4 border-b border-slate-800 cursor-pointer hover:bg-slate-800/50 transition-colors"
            >
              <div className="flex items-center gap-3">
                <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                  <Vibrate size={20} className="text-primary" />
                </div>
                <span className="text-sm font-medium text-slate-200">Vibration Feedback</span>
              </div>
              <div className={`w-12 h-6 rounded-full relative transition-colors ${vibrationEnabled ? 'bg-primary' : 'bg-slate-600'}`}>
                <div className={`absolute top-1 size-4 bg-white rounded-full transition-all ${vibrationEnabled ? 'right-1' : 'left-1'}`} />
              </div>
            </div>

            <div
              onClick={handleCycleSensitivity}
              className="flex items-center justify-between p-4 border-b border-slate-800 cursor-pointer hover:bg-slate-800/50 transition-colors"
            >
              <div className="flex items-center gap-3">
                <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                  <Nfc size={20} className="text-primary" />
                </div>
                <span className="text-sm font-medium text-slate-200">NFC Sensitivity</span>
              </div>
              <div className="flex items-center gap-2 text-slate-400">
                <span className="text-sm">{nfcSensitivity}</span>
                <ChevronRight size={16} />
              </div>
            </div>

            <div className="flex items-center justify-between p-4">
              <div className="flex items-center gap-3">
                <div className="size-10 rounded-lg bg-slate-800 flex items-center justify-center">
                  <Info size={20} className="text-primary" />
                </div>
                <span className="text-sm font-medium text-slate-200">About Us</span>
              </div>
              <div className="flex items-center gap-2 text-slate-400">
                <span className="text-sm">v1.0.0</span>
                <ChevronRight size={16} />
              </div>
            </div>
          </div>
        </section>

        <button
          onClick={handleLogout}
          className="w-full h-14 mt-4 rounded-xl border border-red-500/20 bg-red-500/5 text-red-500 font-bold flex items-center justify-center gap-2 hover:bg-red-500/10 transition-colors"
        >
          <LogOut size={20} />
          Logout
        </button>
      </div>
    </div>
  );
}
