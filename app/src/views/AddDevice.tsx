import { Nfc, LockOpen } from 'lucide-react';
import { View } from '../App';
import BottomNav from '../components/BottomNav';

export default function AddDevice({ onNavigate }: { onNavigate: (view: View) => void }) {
  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden">
      <header className="flex items-center justify-center px-4 py-6">
        <h1 className="text-lg font-bold tracking-tight text-slate-100">Add New Device</h1>
      </header>

      <main className="flex-1 flex flex-col px-6">
        <div className="mt-4 space-y-2">
          <label className="text-sm font-semibold text-slate-400 ml-1" htmlFor="device-nickname">
            Device Nickname
          </label>
          <input 
            id="device-nickname"
            type="text" 
            placeholder="Enter device nickname (e.g., Office)" 
            className="w-full bg-slate-800/50 border-none rounded-xl h-14 px-4 text-base focus:ring-2 focus:ring-primary placeholder:text-slate-600 text-white outline-none transition-all"
          />
        </div>

        <div className="mt-8">
          <button className="w-full bg-primary hover:bg-primary/90 text-white h-16 rounded-xl font-bold text-lg shadow-lg shadow-primary/20 transition-all flex items-center justify-center gap-3">
            <Nfc size={24} />
            Approach and Add
          </button>
        </div>

        <div className="flex-1 flex flex-col items-center justify-center py-12">
          <div className="relative w-64 h-64 flex items-center justify-center rounded-full" style={{ background: 'radial-gradient(circle at center, rgba(19, 127, 236, 0.15) 0%, transparent 70%)' }}>
            
            {/* Phone Mockup */}
            <div className="relative z-10 w-32 h-56 border-4 border-slate-700 rounded-[2.5rem] bg-slate-900 p-1">
              <div className="w-full h-full rounded-[2.2rem] bg-background-dark overflow-hidden flex flex-col items-center pt-4">
                <div className="w-12 h-1 rounded-full bg-slate-700 mb-8"></div>
                <Nfc size={48} className="text-primary opacity-50" />
              </div>
            </div>

            {/* Lock Mockup */}
            <div className="absolute top-0 right-4 w-20 h-20 bg-slate-800 rounded-2xl shadow-xl flex items-center justify-center border border-slate-700">
              <LockOpen size={32} className="text-slate-400" />
            </div>

            {/* Signal Rings */}
            <div className="absolute top-10 right-10 size-16 rounded-full border-2 border-primary/30"></div>
            <div className="absolute top-6 right-6 size-24 rounded-full border-2 border-primary/10"></div>
          </div>

          <div className="mt-8 text-center space-y-2">
            <h3 className="text-base font-bold text-slate-100">Hold phone near the lock</h3>
            <p className="text-sm text-slate-400 max-w-[240px] mx-auto">
              Align the top-back of your phone with the NFC symbol on your passive lock.
            </p>
          </div>
        </div>
      </main>

      <BottomNav currentView="add-device" onNavigate={onNavigate} />
    </div>
  );
}
