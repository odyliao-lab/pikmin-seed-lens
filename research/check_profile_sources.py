"""Compare reviewed native profile records against actual ELF and full-signature dump.
No device operations, no automatic source changes, no compatibility certification.
"""
import argparse
import copy
import json
import re
import struct
from pathlib import Path
import profile_evidence as evidence

def native_profiles(text):
    pattern=r'\{"(\d+\.\d+)",(\d+),(\d+),"([a-f0-9]{64})",(\d+),\s*((?:\{0x[0-9a-f]+,\{(?:0x[0-9a-f]+,?)+\}\},?\s*){5})\}'
    records={}
    for match in re.finditer(pattern,text):
        version,code,size,sha,layout,body=match.groups()
        if version in records:raise ValueError('duplicate profile')
        hooks=re.findall(r'\{(0x[0-9a-f]+),\{([^}]+)\}\}',body)
        records[version]={'version_code':int(code),'size':int(size),'sha':sha,'layout':int(layout),
                          'hooks':{key:(int(rva,16),[int(x,16) for x in words.split(',')])
                                   for key,(rva,words) in zip(evidence.HOOKS,hooks)}}
    if len(records)!=2:raise ValueError('expected two complete reviewed records')
    return records

def verify(profile,result,elf):
    if profile['layout']!=1 or profile['size']!=result['inputs']['elf']['size'] or profile['sha']!=result['inputs']['elf']['sha256']:
        raise ValueError('binary identity mismatch')
    for key,(rva,words) in profile['hooks'].items():
        expected=result['hooks'][key]['rva']
        if key=='text':
            _,branch=evidence.prefix(elf,expected,4)
            if branch!=struct.pack('<I',0x14000001):raise ValueError('text veneer changed')
            expected+=4
        if rva!=expected:raise ValueError('wrong full-signature entry: '+key)
        _,actual=evidence.prefix(elf,rva)
        if list(struct.unpack('<6I',actual))!=words:raise ValueError('prologue mismatch: '+key)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--game1560',type=Path,required=True);p.add_argument('--game1561',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    root=Path(__file__).resolve().parents[1]
    header=root/'native/renderer/game_profiles.h';profiles=native_profiles(header.read_text())
    results={};rejected=0
    for version,folder in [('156.0',a.game1560),('156.1',a.game1561)]:
        result=evidence.evidence(folder/'dump/dump.cs',folder/'libil2cpp.so',folder/'global-metadata.dat')
        profile=profiles[version];elf=folder/'libil2cpp.so'
        verify(profile,result,elf);results[version]=result
        mutations=[]
        for key in profile['hooks']:
            bad=copy.deepcopy(profile);rva,words=bad['hooks'][key];bad['hooks'][key]=(rva+4,words);mutations.append(bad)
            bad=copy.deepcopy(profile);bad['hooks'][key][1][0]^=1;mutations.append(bad)
        for key,value in [('size',profile['size']+1),('sha','0'*64),('layout',2)]:
            bad=copy.deepcopy(profile);bad[key]=value;mutations.append(bad)
        for bad in mutations:
            try:verify(bad,result,elf)
            except ValueError:rejected+=1
            else:raise AssertionError('missed source mutation')
    fields=lambda result:{k:v['fields'] for k,v in result['layouts'].items()}
    if fields(results['156.0'])!=fields(results['156.1']):raise ValueError('shared-layout assumption failed')
    report={'status':'OFFLINE_SOURCE_MATCH_NOT_DEVICE_ACCEPTANCE','header_sha256':evidence.sha(header),
            'shared_layout_classes':len(results['156.0']['layouts']),'rejected_mutations':rejected,'games':results}
    with a.output.open('x',encoding='utf-8') as f:json.dump(report,f,indent=2)
    print(f'PASS: 2 exact ELF identities, 10 full-signature hook bodies/prologues, 2 text veneers, 14 identical class layouts, {rejected} rejected mutations; NOT device acceptance')

if __name__=='__main__':main()
