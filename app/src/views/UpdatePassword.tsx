import { ArrowLeft, EyeOff } from 'lucide-react';
import { View } from '../App';

export default function UpdatePassword({ onNavigate }: { onNavigate: (view: View) => void }) {
  return (
    <div className="flex-1 flex flex-col h-screen overflow-hidden bg-background-dark">
      <header className="flex items-center p-4 pb-2 justify-between sticky top-0 bg-background-dark/80 backdrop-blur-md z-10">
        <button onClick={() => onNavigate('settings')} className="flex size-12 items-center justify-start text-slate-100">
          <ArrowLeft size={24} />
        </button>
        <h2 className="text-lg font-bold flex-1 text-center pr-12">Update Password</h2>
      </header>

      <div className="flex-1 overflow-y-auto px-4 py-6 pb-24 hide-scrollbar">
        <p className="text-sm text-slate-400 mb-8 leading-relaxed">
          Ensure your account is using a long, random password to stay secure. Use at least 8 characters.
        </p>

        <div className="space-y-6">
          <div className="flex flex-col gap-2">
            <label className="text-sm font-semibold text-slate-300">Current Password</label>
            <div className="relative flex items-center">
              <input 
                type="password" 
                placeholder="Enter current password"
                className="w-full bg-slate-800 border border-slate-700 rounded-xl h-14 pl-4 pr-12 text-white focus:outline-none focus:border-primary focus:ring-1 focus:ring-primary placeholder:text-slate-500"
              />
              <button className="absolute right-4 text-slate-500 hover:text-slate-300">
                <EyeOff size={20} />
              </button>
            </div>
          </div>

          <div className="flex flex-col gap-2">
            <label className="text-sm font-semibold text-slate-300">New Password</label>
            <div className="relative flex items-center">
              <input 
                type="password" 
                placeholder="Enter new password"
                className="w-full bg-slate-800 border border-slate-700 rounded-xl h-14 pl-4 pr-12 text-white focus:outline-none focus:border-primary focus:ring-1 focus:ring-primary placeholder:text-slate-500"
              />
              <button className="absolute right-4 text-slate-500 hover:text-slate-300">
                <EyeOff size={20} />
              </button>
            </div>
          </div>

          <div className="flex flex-col gap-2">
            <label className="text-sm font-semibold text-slate-300">Confirm New Password</label>
            <div className="relative flex items-center">
              <input 
                type="password" 
                placeholder="Confirm new password"
                className="w-full bg-slate-800 border border-slate-700 rounded-xl h-14 pl-4 pr-12 text-white focus:outline-none focus:border-primary focus:ring-1 focus:ring-primary placeholder:text-slate-500"
              />
              <button className="absolute right-4 text-slate-500 hover:text-slate-300">
                <EyeOff size={20} />
              </button>
            </div>
          </div>
        </div>
      </div>

      <div className="p-4 border-t border-slate-800/50 bg-background-dark pb-8 absolute bottom-0 w-full">
        <button 
          onClick={() => onNavigate('settings')}
          className="w-full h-14 bg-primary hover:bg-primary/90 text-white font-bold rounded-xl text-base shadow-sm shadow-primary/20 transition-colors"
        >
          Update Password
        </button>
      </div>
    </div>
  );
}
