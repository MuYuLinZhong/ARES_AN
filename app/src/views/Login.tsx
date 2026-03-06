import { ArrowLeft, LockOpen, EyeOff } from 'lucide-react';
import { View } from '../App';

export default function Login({ onNavigate }: { onNavigate: (view: View) => void }) {
  return (
    <div className="flex-1 flex flex-col bg-[#15202b]">
      <div className="flex items-center p-4 pb-2 justify-between">
        <button className="flex size-12 items-center justify-center text-slate-300 hover:text-primary transition-colors">
          <ArrowLeft size={24} />
        </button>
        <h2 className="text-lg font-bold flex-1 text-center pr-12">Login</h2>
      </div>

      <div className="flex-1 overflow-y-auto px-6 py-8 flex flex-col justify-center">
        <div className="flex justify-center mb-10">
          <div className="size-24 rounded-full bg-primary/10 flex items-center justify-center text-primary">
            <LockOpen size={48} strokeWidth={1.5} />
          </div>
        </div>
        
        <h1 className="text-2xl font-bold text-center mb-2">Welcome Back</h1>
        <p className="text-slate-400 text-center mb-8 text-sm">Securely manage your NFC smart locks</p>

        <div className="space-y-4 mb-6">
          <label className="flex flex-col">
            <span className="text-sm font-medium pb-1.5 text-slate-300">Phone Number</span>
            <input 
              type="tel" 
              placeholder="+1 (555) 000-0000"
              className="w-full bg-[#1e2a38] border border-slate-700 rounded-xl h-14 px-4 text-white focus:outline-none focus:border-primary focus:ring-1 focus:ring-primary placeholder:text-slate-500"
            />
          </label>
          
          <label className="flex flex-col">
            <span className="text-sm font-medium pb-1.5 text-slate-300">Password</span>
            <div className="relative flex items-center">
              <input 
                type="password" 
                placeholder="Enter your password"
                className="w-full bg-[#1e2a38] border border-slate-700 rounded-xl h-14 pl-4 pr-12 text-white focus:outline-none focus:border-primary focus:ring-1 focus:ring-primary placeholder:text-slate-500"
              />
              <button className="absolute right-4 text-slate-500 hover:text-slate-300">
                <EyeOff size={20} />
              </button>
            </div>
          </label>
        </div>

        <div className="flex justify-end mb-8">
          <button className="text-primary text-sm font-medium hover:underline">Forgot Password?</button>
        </div>

        <div className="space-y-4">
          <button 
            onClick={() => onNavigate('home')}
            className="w-full h-14 bg-primary hover:bg-primary/90 text-white font-bold rounded-xl text-lg shadow-lg shadow-primary/25 transition-colors"
          >
            Login
          </button>
          
          <div className="flex items-center justify-center gap-2 pt-4">
            <span className="text-slate-400 text-sm">Don't have an account?</span>
            <button className="text-primary text-sm font-bold hover:underline">Sign Up</button>
          </div>
        </div>
      </div>
    </div>
  );
}
