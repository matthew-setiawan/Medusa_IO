# Medusa_IO
EIE2 InfoProcessing Coursework group 13

## pip packages
To run the project, following pip packages must be installed
- `intel_jtag_uart`
- `tkinter`
- `tk`

## Notes

Working for one food only
We are currently deleting all other foods
Sometimes bugs and food no longer generates

# Networking allocation UDP

In order to setup the networking on Windows go to Windows Defender Firewall with Advanced Security and perform two following steps:

- Go to `Inbound rules`
  - Click on `New rule`
  - Select `Port` and click `Next`
  - Select `UDP`,set the inbound port and click `Next`
  - Select `Allow the connection` and click `Next`
  - Leave everything as it is and click `Next`
  - Name the rule and click `Finish`
- Go to `Outbound rules`
  - Click on `New rule`
  - Select `Port` and click `Next`
  - Select `UDP`,set the inbound port and click `Next`
  - Select `Allow the connection` and click `Next`
  - Leave everything as it is and click `Next`
  - Name the rule and click `Finish`

The following setup was introduced for each member of the team. 

Alex: inbound - 12000 (local main server); outbound - 514 (client side server)
Vaclav: inbound - 12010 (local main server); outbound - 515 (client side server)
James: inbound - 12020 (local main server); outbound - 516 (client side server)