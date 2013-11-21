/***************************************************************************
   sim_signals.h   -   signals for messaging and error system

   Christoph Kirst
   christoph@nld.ds.mpg.de 
   Max Planck Institue for Dynamics and Self-Organisation Göttingen
   HU Berlin, BCCN Göttingen & Berlin (2008)
****************************************************************************/
#ifndef SIM_SIGNALS_H
#define SIM_SIGNALS_H


enum SimSignal {
                Success = 0,
                AllOutput,
                Message,
                Msg = Message,
                NoMessage,
                Warning,
                NoWarning,
                Abort,
                NoAbort,
                Exit,
                NoExit,
                Invalid,
                NoOutput
               };

bool is_io_signal(const int& sig);
bool is_error_signal(const int& sig);
bool is_throw_signal(const int& sig); 
bool is_io_output_signal(const int& sig);
SimSignal to_output_signal(const SimSignal& sig);

#endif
