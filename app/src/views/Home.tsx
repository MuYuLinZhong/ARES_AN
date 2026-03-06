import { useState, useEffect } from 'react';
import { Bell, LockOpen, Lock, Nfc } from 'lucide-react';
import { View } from '../App';
import BottomNav from '../components/BottomNav';

export default function Home({ onNavigate }: { onNavigate: (view: View) => void }) {
  const [isUnlocking, setIsUnlocking] = useState(false);
  const [progress, setProgress] = useState(0);

  useEffect(() => {
    if (isUnlocking) {
      const interval = setInterval(() => {
        setProgress(p => {
          if (p >= 100) {
            clearInterval(interval);
            setTimeout(() => setIsUnlocking(false), 500);
            return 100;
          }
          return p + 5;
        });
      }, 100);
      return () => clearInterval(interval);
    } else {
      setProgress(0);
    }
  }, [isUnlocking]);

  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden relative">
      <header className="flex items-center justify-between px-6 pt-14 pb-4 z-10 shrink-0">
        <button 
          onClick={() => onNavigate('settings')}
          className="size-10 rounded-full bg-slate-800 border-2 border-primary/20 overflow-hidden shrink-0"
        >
          <img src="https://picsum.photos/seed/alex/100/100" alt="Profile" className="w-full h-full object-cover" />
        </button>
        <div className="flex flex-col items-center flex-1">
          <span className="text-[10px] font-bold text-slate-500 uppercase tracking-[0.2em] mb-0.5">Active Device</span>
          <h2 className="text-lg font-extrabold tracking-tight">Main Gate</h2>
        </div>
        <button className="size-10 shrink-0 flex items-center justify-center text-slate-400">
          <Bell size={24} />
        </button>
      </header>

      <main className="flex-1 flex flex-col items-center justify-center px-6 relative mb-24">
        <div className="relative flex flex-col items-center">
          <div className="relative flex items-center justify-center w-64 h-64 rounded-full border-4 border-primary/30 bg-gradient-to-tr from-primary/10 to-transparent shadow-[0_0_60px_10px_rgba(19,127,236,0.3)]">
            <div className="absolute inset-4 rounded-full border-2 border-dashed border-primary/40 animate-[spin_10s_linear_infinite]"></div>
            <div className="flex flex-col items-center gap-4 z-10">
              <div className="w-20 h-20 rounded-full bg-primary flex items-center justify-center shadow-lg shadow-primary/40">
                <Nfc size={40} className="text-white" strokeWidth={1.5} />
              </div>
            </div>
          </div>
          <div className="mt-12 text-center">
            <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Ready to Unlock</h1>
            <p className="mt-2 text-slate-400 text-sm font-medium">Scanning for authorized NFC signal...</p>
          </div>
        </div>
      </main>

      <section className="px-6 pb-28 z-10 w-full shrink-0">
        <div className="flex gap-4 mb-4">
          <button 
            onClick={() => setIsUnlocking(true)}
            className="flex-1 h-16 flex items-center justify-center gap-2 rounded-xl bg-primary hover:bg-primary/90 text-white font-bold text-lg shadow-lg shadow-primary/25 transition-colors"
          >
            <LockOpen size={24} />
            Unlock
          </button>
          <button className="flex-1 h-16 flex items-center justify-center gap-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-slate-200 font-bold text-lg transition-colors">
            <Lock size={24} />
            Lock
          </button>
        </div>
        <p className="text-center text-xs font-medium text-slate-500 uppercase tracking-widest">
          Keep NFC close after clicking
        </p>
      </section>

      <BottomNav currentView="home" onNavigate={onNavigate} />

      {/* Unlocking Sheet Overlay */}
      {isUnlocking && (
        <>
          <div 
            className="fixed inset-0 bg-black/60 backdrop-blur-sm z-40"
            onClick={() => setIsUnlocking(false)}
          ></div>
          <div className="fixed bottom-0 left-0 right-0 h-[55vh] bg-[#1a2533] rounded-t-[32px] z-50 shadow-[0_-8px_30px_rgba(0,0,0,0.3)] flex flex-col items-center px-8 animate-in slide-in-from-bottom duration-300">
            <div className="w-12 h-1.5 bg-slate-700 rounded-full mt-4 mb-10"></div>
            
            <div className="flex-1 flex flex-col items-center justify-center w-full mb-10">
              <div className="relative w-32 h-32 flex items-center justify-center mb-8">
                <svg className="w-full h-full -rotate-90">
                  <circle cx="64" cy="64" r="56" fill="transparent" stroke="currentColor" strokeWidth="8" className="text-slate-800" />
                  <circle 
                    cx="64" cy="64" r="56" 
                    fill="transparent" 
                    stroke="currentColor" 
                    strokeWidth="8" 
                    strokeLinecap="round"
                    className="text-primary transition-all duration-300"
                    style={{ strokeDasharray: 351.8, strokeDashoffset: 351.8 - (351.8 * progress) / 100 }}
                  />
                </svg>
                <div className="absolute flex flex-col items-center justify-center">
                  <span className="text-2xl font-extrabold text-primary">{progress}%</span>
                </div>
              </div>
              
              <h3 className="text-xl font-extrabold tracking-tight mb-2">Unlocking device...</h3>
              <p className="text-slate-400 text-center font-medium max-w-[280px]">
                Scanning for NFC signal. Please keep your phone close to the reader.
              </p>
              
              <button 
                onClick={() => setIsUnlocking(false)}
                className="mt-8 px-8 py-3 bg-slate-800 rounded-full text-sm font-bold text-slate-300 active:scale-95 transition-transform"
              >
                Cancel
              </button>
            </div>
          </div>
        </>
      )}
    </div>
  );
}
